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

    size_t Size() const;
    void CancelWait();
    void Push(const Chunk& buffer);
    Chunk Pop(const size_t size);

  private:
    bool Cancel;
    const size_t MaxSize;
    size_t TheSize;
    size_t Offset;
    std::deque<Chunk> Deque;
    mutable boost::mutex LockDeque;
    boost::condition_variable NewChunksArrived;
    boost::condition_variable PopProcessed;
  };
}