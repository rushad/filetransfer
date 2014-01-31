#pragma once

#include <boost/thread/condition_variable.hpp>
#include <boost/thread/lock_guard.hpp>
#include <boost/thread/mutex.hpp>

#include <queue>
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
    void Cancel();
    void Push(const Chunk& buffer);
    Chunk Pop(const size_t size);

  private:
    typedef boost::lock_guard<boost::mutex> Lock;
    typedef boost::unique_lock<boost::mutex> CondLock;

    void WaitData(CondLock& lock);

    bool Stop;
    const size_t MaxSize;
    size_t TheSize;
    size_t Offset;
    std::queue<Chunk> Container;
    mutable boost::mutex LockQueue;
    boost::condition_variable PushProcessed;
    boost::condition_variable PopProcessed;
  };
}