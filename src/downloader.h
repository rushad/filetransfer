#pragma once

#include "source.h"
#include "queue.h"
#include "receiver.h"

namespace FileTransfer
{
  class Downloader : public Receiver
  {
  public:
    Downloader(Source& src, Queue& q);
    void Start();
    virtual void Receive(const void *buffer, size_t size, size_t nmemb);

  private:
    static void* ThreadFunc(void *data);

    Source& Src;
    Queue& Q;
  };
}