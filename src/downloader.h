#pragma once

#include "observer.h"
#include "queue.h"
#include "receiver.h"
#include "source.h"

#include <pthread.h>

namespace FileTransfer
{
  class Downloader : public Receiver
  {
  public:
    Downloader(Source& src, Queue& q, Observer* obs = 0);
    ~Downloader();
    void Start();
    void Cancel();
    State Wait(const unsigned ms = UINT_MAX);
    std::string Error() const;
    boost::uint64_t Size() const;

    virtual bool Cancelled() const;
    virtual void Receive(const void *buffer, size_t size, size_t nmemb);

  private:
    static void* ThreadFunc(void *data);

    pthread_t ThreadId;
    Source& Src;
    Queue& Q;
    Observer* Obs;
    boost::uint64_t SrcSize;
    boost::uint64_t Downloaded;
    unsigned LastProgress;

    mutable boost::mutex LockStop;
    bool Stop;

    mutable boost::mutex LockResult;
    bool Result;
    bool Done;
    boost::condition_variable CondDone;

    std::string SrcError;
  };
}