#pragma once

#include "downloader.h"
#include "observer.h"
#include "queue.h"
#include "source.h"
#include "target.h"
#include "uploader.h"

#include <limits.h>

#include <string>

namespace FileTransfer
{
  class FileTransfer
  {
  public:
    const static unsigned WAIT_INFINITE = UINT_MAX;
    FileTransfer(const std::string& urlSrc, const std::string& urlTrg, Observer* srcObs = 0, Observer* trgObs = 0);
    FileTransfer(const Source::Ptr& src, const Target::Ptr& trg, Observer* srcObs = 0, Observer* trgObs = 0);
    void Start();
    void Wait(const unsigned timeout = WAIT_INFINITE);
    void Exec(const unsigned timeout = WAIT_INFINITE);
    void Cancel();

  private:
    Queue Q;
    Source::Ptr Src;
    Target::Ptr Trg;
    Downloader Dl;
    Uploader Ul;
  };
}