#pragma once

#include "curl.h"
#include "downloader.h"
#include "queue.h"
#include "uploader.h"

#include <string>

namespace FileTransfer
{
  class FileTransfer
  {
  public:
    FileTransfer(const std::string& urlSrc, const std::string& urlTrg);
    void Transfer(const unsigned timeout = UINT_MAX);
    void Cancel();

  private:
    Queue Q;
    CurlSource Src;
    CurlTarget Trg;
    Downloader Dl;
    Uploader Ul;
  };
}