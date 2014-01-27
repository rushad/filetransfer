#include "filetransfer.h"

namespace FileTransfer
{
  FileTransfer::FileTransfer(const std::string& urlSrc, const std::string& urlTrg)
    : Src(urlSrc)
    , Trg(urlTrg)
    , Dl(Src, Q)
    , Ul(Trg, Q, Src.GetSize())
  {
  }

  void FileTransfer::Transfer(const unsigned timeout)
  {
    Dl.Start();
    Ul.Start();

    Ul.Wait(timeout);
  }

  void FileTransfer::Cancel()
  {
    Dl.Cancel();
    Ul.Cancel();
  }
}