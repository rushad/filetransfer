#pragma once

namespace FileTransfer
{
  class Receiver
  {
  public:
    virtual ~Receiver() {}
    virtual bool Cancelled() const = 0;
    virtual void Receive(const void *buffer, size_t size, size_t nmemb) = 0;
  };
}
