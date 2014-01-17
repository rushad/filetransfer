#include "queue.h"

namespace FileTransfer
{
  Queue::Queue(const size_t maxSize)
    : Cancel(false)
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
    boost::lock_guard<boost::mutex> locker(LockDeque);
    return TheSize;
  }

  void Queue::CancelWait()
  {
    {
      boost::lock_guard<boost::mutex> locker(LockDeque);
      Cancel = true;
    }
    NewChunksArrived.notify_one();
    PopProcessed.notify_one();
  }

  void Queue::Push(const Chunk& chunk)
  {
    {
      boost::unique_lock<boost::mutex> locker(LockDeque);
      if (MaxSize)
      {
        Cancel = false;
        while (!Cancel && (TheSize >= MaxSize))
          PopProcessed.wait(locker);
      }
      Deque.push_back(chunk);
      TheSize += chunk.size();
    }
    NewChunksArrived.notify_one();
  }

  Queue::Chunk Queue::Pop(const size_t size)
  {
    Chunk result;

    boost::unique_lock<boost::mutex> locker(LockDeque);

    if (Deque.empty())
    {
      Cancel = false;
      while (!Cancel && Deque.empty())
      {
        NewChunksArrived.wait(locker);
      }
      if (Cancel)
        return result;
    }

    while (!Deque.empty() && (result.size() < size))
    {
      Chunk chunk = Deque.front();

      const size_t toCopy = std::min(size - result.size(), chunk.size() - Offset);

      result.insert(result.end(), chunk.begin() + Offset, chunk.begin() + Offset + toCopy);

      Offset += toCopy;
      if(Offset == chunk.size())
      {
        Deque.pop_front();
        TheSize -= chunk.size();
        Offset = 0;
        PopProcessed.notify_one();
      }
    } 

    return result;
  }

}