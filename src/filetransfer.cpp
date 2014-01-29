#include "curl.h"
#include "filetransfer.h"

namespace FileTransfer
{
  FileTransfer::FileTransfer(const std::string& urlSrc, const std::string& urlTrg, Observer* srcObs, Observer* trgObs)
    : Src(new CurlSource(urlSrc))
    , Trg(new CurlTarget(urlTrg))
    , Dl(*Src, Q, srcObs)
    , Ul(*Trg, Q, Dl.Size(), trgObs)
  {
  }

  FileTransfer::FileTransfer(Source::Ptr src, Target::Ptr trg, Observer* srcObs, Observer* trgObs)
    : Src(src)
    , Trg(trg)
    , Dl(*Src, Q, srcObs)
    , Ul(*Trg, Q, Dl.Size(), trgObs)
  {
  }

  void FileTransfer::Start()
  {
    Dl.Start();
    Ul.Start();
  }

  void FileTransfer::Wait(const unsigned timeout)
  {
    Ul.Wait(timeout);
  }

  void FileTransfer::Exec(const unsigned timeout)
  {
    Start();
    Wait(timeout);
  }

  void FileTransfer::Cancel()
  {
    Dl.Cancel();
    Ul.Cancel();
  }
}