#include "curl.h"
#include "multiple_filetransfer.h"

namespace FileTransfer
{
  MultipleFileTransfer::MultipleFileTransfer(const UrlVector urlSrc, const UrlVector urlTrg, Observer* obs)
    : Obs(obs)
    , LastDlProgress(0)
    , LastUlProgress(0)
    , TotalSize(0)
  {
    if (urlSrc.size() != urlTrg.size())
      throw std::exception("MultipleFileTransfer ctor: sources and targets count mismatched");
    if (!urlSrc.size() || !urlTrg.size())
      throw std::exception("MultipleFileTransfer ctor: empty sources list");
    if(!urlTrg.size())
      throw std::exception("MultipleFileTransfer ctor: empty targets list");

    UrlVector::const_iterator itSrc = urlSrc.begin();
    UrlVector::const_iterator itTrg = urlTrg.begin();
    for (; itSrc != urlSrc.end(); ++itSrc, ++itTrg)
    {
      Source::Ptr src(new CurlSource(*itSrc));
      Target::Ptr trg(new CurlTarget(*itTrg));
      Queue::Ptr q(new Queue);
      Sources.push_back(src);
      Targets.push_back(trg);
      Queues.push_back(q);
      Downloader::Ptr dl(new Downloader(*src, *q));
      Uploader::Ptr ul(new Uploader(*trg, *q, dl->Size()));
      Downloaders.push_back(dl);
      Uploaders.push_back(ul);
      TotalSize += dl->Size();
    }
  }

  MultipleFileTransfer::MultipleFileTransfer(const Source::Vector src, const Target::Vector trg, Observer* obs)
    : Sources(src)
    , Targets(trg)
    , Obs(obs)
    , LastDlProgress(0)
    , LastUlProgress(0)
    , TotalSize(0)
  {
    if (src.size() != trg.size())
      throw std::exception("MultipleFileTransfer ctor: sources and targets count mismatched");
    if (!src.size() || !trg.size())
      throw std::exception("MultipleFileTransfer ctor: empty sources list");
    if(!trg.size())
      throw std::exception("MultipleFileTransfer ctor: empty targets list");

    Source::Vector::const_iterator itSrc = src.begin();
    Target::Vector::const_iterator itTrg = trg.begin();
    for (; itSrc != src.end(); ++itSrc, ++itTrg)
    {
      Queue::Ptr q(new Queue);
      Queues.push_back(q);
      Downloader::Ptr dl(new Downloader(**itSrc, *q));
      Uploader::Ptr ul(new Uploader(**itTrg, *q, dl->Size()));
      Downloaders.push_back(dl);
      Uploaders.push_back(ul);
      TotalSize += dl->Size();
    }
  }

  void MultipleFileTransfer::Start()
  {
    DlObs = BytesObserver::Ptr(new BytesObserver(*this));
    UlObs = BytesObserver::Ptr(new BytesObserver(*this));

    Downloader::Vector::const_iterator itDl = Downloaders.begin();
    Uploader::Vector::const_iterator itUl = Uploaders.begin();
    
    for (; itDl != Downloaders.end(); ++itDl, ++itUl)
    {
      (*itDl)->SetObserver(DlObs);
      (*itDl)->Start();
      (*itUl)->SetObserver(UlObs);
      (*itUl)->Start();
    }
  }

  void MultipleFileTransfer::Wait(const unsigned timeout)
  {
    typedef boost::chrono::system_clock time;
    time::time_point te = time::now() + boost::chrono::milliseconds(timeout);

    Uploader::Vector::const_iterator itUl = Uploaders.begin();
    
    for (; itUl != Uploaders.end(); ++itUl)
    {
      time::time_point t = time::now();
      if (t >= te)
        break;
      unsigned ms = (unsigned)boost::chrono::duration_cast<boost::chrono::milliseconds>(te - t).count();
      (*itUl)->Wait(ms);
    }
  }

  void MultipleFileTransfer::Cancel()
  {
    Downloader::Vector::const_iterator itDl = Downloaders.begin();
    Uploader::Vector::const_iterator itUl = Uploaders.begin();
    
    for (; itDl != Downloaders.end(); ++itDl, ++itUl)
    {
      (*itDl)->Cancel();
      (*itUl)->Cancel();
    }
  }

  void MultipleFileTransfer::Calculate()
  {
    unsigned dlProgress = (unsigned)((double)DlObs->GetBytes() / TotalSize * 100);
    unsigned ulProgress = (unsigned)((double)UlObs->GetBytes() / TotalSize * 100);
    if (Obs && ((dlProgress != LastDlProgress) || (ulProgress != LastUlProgress)))
    {
      Obs->UpdateProgress(dlProgress, ulProgress);
    }
    LastDlProgress = dlProgress;
    LastUlProgress = ulProgress;
  }

  void MultipleFileTransfer::Exec(const unsigned timeout)
  {
    Start();
    Wait(timeout);
  }

  void MultipleFileTransfer::Wait()
  {
    Wait(WAIT_INFINITE);
  }
}