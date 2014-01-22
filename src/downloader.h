#pragma once

#include "queue.h"
#include "receiver.h"
#include "source.h"

#include <pthread.h>

namespace FileTransfer
{
  class Downloader : public Receiver
  {
  public:
    Downloader(Source& src, Queue& q);
    ~Downloader();
    void Start();
    void Cancel();
    bool Wait();

    virtual void Receive(const void *buffer, size_t size, size_t nmemb);
    virtual bool Cancelled() const;

  private:
    static void* ThreadFunc(void *data);

    pthread_t ThreadId;
    Source& Src;
    Queue& Q;

    mutable boost::mutex LockStop;
    bool Stop;
    mutable boost::mutex LockDone;
    bool Done;
    bool Result;
  };
}