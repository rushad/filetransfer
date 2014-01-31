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
#include <vector>

namespace FileTransfer
{
  class MultipleFileTransfer : public FileTransfer, public ProgressCalculator
  {
  public:
    typedef std::vector<std::string> UrlVector;
    MultipleFileTransfer(const UrlVector urlSrc, const UrlVector urlTrg, Observer* = 0);
    MultipleFileTransfer(const Source::Vector src, const Target::Vector trg, Observer* = 0);
    virtual void Start();
    virtual void Wait(const unsigned timeout);
    virtual void Cancel();
    virtual void Calculate();
    void Exec(const unsigned timeout = WAIT_INFINITE);
    void Wait();

  private:
    Queue::Vector Queues;
    Source::Vector Sources;
    Target::Vector Targets;
    BytesObserver::Ptr DlObs;
    BytesObserver::Ptr UlObs;
    Downloader::Vector Downloaders;
    Uploader::Vector Uploaders;
    Observer* Obs;
    boost::uint64_t TotalSize;
    unsigned LastDlProgress;
    unsigned LastUlProgress;
  };
}