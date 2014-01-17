#pragma once

#include <boost/shared_array.hpp>
#include <boost/thread/condition_variable.hpp>

#include <queue>
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
    typedef boost::lock_guard<boost::mutex> Lock;
    typedef boost::unique_lock<boost::mutex> CondLock;

    void WaitData(CondLock& lock);

    bool Cancel;
    const size_t MaxSize;
    size_t TheSize;
    size_t Offset;
    std::queue<Chunk> Container;
    mutable boost::mutex LockQueue;
    boost::condition_variable PushProcessed;
    boost::condition_variable PopProcessed;
  };
}