#pragma once

namespace FileTransfer
{
  class Receiver
  {
  public:
    virtual ~Receiver() {}
    virtual void Receive(const void *buffer, size_t size, size_t nmemb) = 0;
    virtual bool Cancelled() const = 0;
  };
}
