#pragma once

#include "queue.h"
#include "transmitter.h"
#include "target.h"

#include <pthread.h>

namespace FileTransfer
{
  class Uploader : public Transmitter
  {
  public:
    Uploader(Target& trg, Queue& q, const boost::uint64_t length);
    ~Uploader();
    void Start();
    void Cancel();
    virtual size_t Transmit(void *buffer, size_t size, size_t nmemb);
    virtual bool Cancelled() const;

  private:
    static void* ThreadFunc(void *data);

    pthread_t ThreadId;
    Target& Trg;
    Queue& Q;
    const boost::uint64_t Length;
    boost::uint64_t Uploaded;

    mutable boost::mutex LockStop;
    bool Stop;
  };
}