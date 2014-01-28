#include "curl.h"
#include "filetransfer.h"

namespace FileTransfer
{
  FileTransfer::FileTransfer(const std::string& urlSrc, const std::string& urlTrg)
    : Src(new CurlSource(urlSrc))
    , Trg(new CurlTarget(urlTrg))
    , Dl(*Src, Q)
    , Ul(*Trg, Q, Src->GetSize())
  {
  }

  FileTransfer::FileTransfer(Source* src, Target* trg)
    : Src(src)
    , Trg(trg)
    , Dl(*Src, Q)
    , Ul(*Trg, Q, Src->GetSize())
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