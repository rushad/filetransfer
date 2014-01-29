#pragma once

#include "observer.h"
#include "queue.h"
#include "transmitter.h"
#include "target.h"

#include <pthread.h>

namespace FileTransfer
{
  class Uploader : public Transmitter
  {
  public:
    Uploader(Target& trg, Queue& q, const boost::uint64_t length, Observer* obs = 0);
    ~Uploader();
    void Start();
    void Cancel();
    State Wait(const unsigned ms = UINT_MAX);
    std::string Error() const;

    virtual bool Cancelled() const;
    virtual size_t Transmit(void *buffer, size_t size, size_t nmemb);

  private:
    static void* ThreadFunc(void *data);

    pthread_t ThreadId;
    Target& Trg;
    Queue& Q;
    Observer* Obs;
    const boost::uint64_t SrcSize;
    boost::uint64_t Uploaded;
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