#pragma once

#include "bytes_observer.h"
#include "queue.h"
#include "receiver.h"
#include "source.h"
#include "transfer.h"

#include <pthread.h>

#include <boost/cstdint.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>

#include <string>

namespace FileTransfer
{
  class Downloader : public Receiver
  {
  public:
    Downloader(Source& src, Queue& q);
    ~Downloader();
    void SetObserver(BytesObserver::Ptr obs);
    void Start();
    void Cancel();
    TransferState Wait(const unsigned ms = UINT_MAX);
    std::string Error() const;
    boost::uint64_t Size() const;

    virtual bool Cancelled() const;
    virtual void Receive(const void *buffer, size_t size, size_t nmemb);

  private:
    static void* ThreadFunc(void *data);

    pthread_t ThreadId;
    Source& Src;
    Queue& Q;
    BytesObserver::Ptr Obs;
    boost::uint64_t SrcSize;

    mutable boost::mutex LockStop;
    bool Stop;

    mutable boost::mutex LockResult;
    bool Result;
    bool Done;
    boost::condition_variable CondDone;

    std::string SrcError;
  };
}