#pragma once

#include "downloader.h"
#include "receiver.h"
#include "source.h"

namespace FileTransfer
{
  class CurlSource : public Source
  {
  public:
    void Run(Receiver& wr);

  private:
    static size_t WriteFunc(void *buffer, size_t size, size_t nmemb, void *data);
  };

}