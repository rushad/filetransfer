#pragma once

#include "bytes_observer.h"
#include "downloader.h"
#include "filetransfer.h"
#include "observer.h"
#include "queue.h"
#include "source.h"
#include "progress_calculator.h"
#include "target.h"
#include "uploader.h"

#include <string>

namespace FileTransfer
{
  class SingleFileTransfer : public FileTransfer, public ProgressCalculator
  {
  public:
    SingleFileTransfer(const std::string& urlSrc, const std::string& urlTrg, Observer* = 0);
    SingleFileTransfer(const Source::Ptr& src, const Target::Ptr& trg, Observer* = 0);
    virtual void Start();
    virtual void Wait(const unsigned timeout);
    virtual void Cancel();
    virtual void Calculate();
    void Exec(const unsigned timeout = WAIT_INFINITE);
    void Wait();

  private:
    Queue Q;
    Source::Ptr Src;
    Target::Ptr Trg;
    Downloader Dl;
    Uploader Ul;
    Observer* Obs;
    BytesObserver::Ptr DlObs;
    BytesObserver::Ptr UlObs;
    unsigned LastDlProgress;
    unsigned LastUlProgress;
  };
}