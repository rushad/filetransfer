#include "queue.h"

namespace FileTransfer
{
  Queue::Queue(const size_t maxSize)
    : Stop(false)
    , MaxSize(maxSize)
    , TheSize(0)
    , Offset(0)
  {
  }

  Queue::~Queue()
  {
  }

  size_t Queue::Size() const
  {
    Lock lock(LockQueue);
    return TheSize;
  }

  void Queue::Cancel()
  {
    {
      Lock lock(LockQueue);
      Stop = true;
    }
    PushProcessed.notify_one();
    PopProcessed.notify_one();
  }

  void Queue::Push(const Chunk& chunk)
  {
    {
      CondLock lock(LockQueue);
      if (MaxSize)
      {
        while (!Stop && (TheSize >= MaxSize))
          PopProcessed.wait(lock);
      }
      Container.push(chunk);
      TheSize += chunk.size();
    }
    PushProcessed.notify_one();
  }

  void Queue::WaitData(CondLock& lock)
  {
    if (Container.empty())
    {
      while (!Stop && Container.empty())
      {
        PushProcessed.wait(lock);
      }
    }
  }

  Queue::Chunk Queue::Pop(const size_t size)
  {
    Chunk result;

    CondLock lock(LockQueue);

    WaitData(lock);

    if (Stop)
      return result;

    while (!Container.empty() && (result.size() < size))
    {
      Chunk chunk = Container.front();

      const size_t toCopy = std::min(size - result.size(), chunk.size() - Offset);

      result.insert(result.end(), chunk.begin() + Offset, chunk.begin() + Offset + toCopy);

      Offset += toCopy;
      if(Offset == chunk.size())
      {
        Container.pop();
        TheSize -= chunk.size();
        Offset = 0;
        PopProcessed.notify_one();
      }
    } 

    return result;
  }

}