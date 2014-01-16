#include "queue.h"

namespace FileTransfer
{
  Queue::Queue(const size_t maxSize)
    : Stop(false)
    , MaxSize(maxSize)
    , Size(0)
    , Offset(0)
  {
  }

  Queue::~Queue()
  {
    Cancel();
  }

  void Queue::Cancel()
  {
    Stop = true;
    NewChunksArrived.notify_all();
  }

  void Queue::Push(const Chunk& chunk)
  {
    {
      boost::unique_lock<boost::mutex> locker(LockDeque);
      Deque.push_back(chunk);
    }
    NewChunksArrived.notify_one();
  }

  Queue::Chunk Queue::Pop(const size_t size)
  {
    Chunk result;

    if (Deque.empty())
    {
      boost::unique_lock<boost::mutex> locker(LockDeque);
      while (!Stop && Deque.empty())
      {
        NewChunksArrived.wait(locker);
      }
      if (Stop)
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
        Offset = 0;
      }

    } 

    return result;
  }

}