#pragma once

namespace FileTransfer
{
  class Transmitter
  {
  public:
    virtual ~Transmitter() {}
    virtual bool Cancelled() const = 0;
    virtual size_t Transmit(void *buffer, size_t size, size_t nmemb) = 0;
  };
}
