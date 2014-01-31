#include "curl.h"
#include "single_filetransfer.h"

namespace FileTransfer
{
  SingleFileTransfer::SingleFileTransfer(const std::string& urlSrc, const std::string& urlTrg, Observer* obs)
    : Src(new CurlSource(urlSrc))
    , Trg(new CurlTarget(urlTrg))
    , Dl(*Src, Q)
    , Ul(*Trg, Q, Dl.Size())
    , Obs(obs)
    , LastDlProgress(0)
    , LastUlProgress(0)
  {
  }

  SingleFileTransfer::SingleFileTransfer(const Source::Ptr& src, const Target::Ptr& trg, Observer* obs)
    : Src(src)
    , Trg(trg)
    , Dl(*Src, Q)
    , Ul(*Trg, Q, Dl.Size())
    , Obs(obs)
    , LastDlProgress(0)
    , LastUlProgress(0)
  {
  }

  void SingleFileTransfer::Start()
  {
    DlObs = BytesObserver::Ptr(new BytesObserver(*this));
    Dl.SetObserver(DlObs);
    Dl.Start();

    UlObs = BytesObserver::Ptr(new BytesObserver(*this));
    Ul.SetObserver(UlObs);
    Ul.Start();
  }

  void SingleFileTransfer::Wait(const unsigned timeout)
  {
    Ul.Wait(timeout);
  }

  void SingleFileTransfer::Cancel()
  {
    Dl.Cancel();
    Ul.Cancel();
  }

  void SingleFileTransfer::Calculate()
  {
    unsigned dlProgress = (unsigned)((double)DlObs->GetBytes() / Dl.Size() * 100);
    unsigned ulProgress = (unsigned)((double)UlObs->GetBytes() / Dl.Size() * 100);
    if (Obs && ((dlProgress != LastDlProgress) || (ulProgress != LastUlProgress)))
    {
      Obs->UpdateProgress(dlProgress, ulProgress);
    }
    LastDlProgress = dlProgress;
    LastUlProgress = ulProgress;
  }

  void SingleFileTransfer::Exec(const unsigned timeout)
  {
    Start();
    Wait(timeout);
  }

  void SingleFileTransfer::Wait()
  {
    Wait(WAIT_INFINITE);
  }
}