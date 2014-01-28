#pragma once

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
    FileTransfer(Source* src, Target* trg);
    void Start();
    void Wait(const unsigned timeout = UINT_MAX);
    void Exec(const unsigned timeout = UINT_MAX);
    void Cancel();

  private:
    Queue Q;
    Source* Src;
    Target* Trg;
    Downloader Dl;
    Uploader Ul;
  };
}