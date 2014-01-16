#pragma once

#include <boost/shared_array.hpp>
#include <boost/thread/condition_variable.hpp>

#include <deque>
#include <memory>
#include <vector>

namespace FileTransfer
{
  const size_t DefaultMaxQueueSize = 1024 * 1024 * 1024;
  class Queue
  {
  public:
    typedef std::vector<unsigned char> Chunk;

    explicit Queue(const size_t maxSize = DefaultMaxQueueSize);
    ~Queue();

    void Cancel();
    void Push(const Chunk& buffer);
    Chunk Pop(const size_t size);

  private:
    bool Stop;
    const size_t MaxSize;
    size_t Size;
    size_t Offset;
    std::deque<Chunk> Deque;
    boost::mutex LockDeque;
    boost::condition_variable NewChunksArrived;
  };
}