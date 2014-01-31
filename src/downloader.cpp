#include "downloader.h"

namespace FileTransfer
{
  Downloader::Downloader(Source& src, Queue& q)
    : ThreadId(pthread_self())
    , Src(src)
    , Q(q)
    , Obs(0)
    , SrcSize(src.GetSize())
    , Stop(false)
    , Result(false)
    , Done(false)
  {
  }

  Downloader::~Downloader()
  {
    if (!pthread_equal(ThreadId, pthread_self()))
      pthread_join(ThreadId, 0);
  }

  void Downloader::SetObserver(BytesObserver::Ptr obs)
  {
    Obs = obs;
  }

  void Downloader::Start()
  {
    int r = pthread_create(&ThreadId, 0, ThreadFunc, this);
    switch(r)
    {
    case 0:
      break;
    case EAGAIN:
      throw std::exception("Downloader::Start(): EAGAIN: Insufficient resources to create thread");
    case EINVAL:
      throw std::exception("Downloader::Start(): EINVAL: Invalid settings in attr");
    case EPERM:
      throw std::exception("Downloader::Start(): EINVAL: No permission to set the scheduling policy and parameters");
    default:
      throw std::exception("Downloader::Start(): Unknown error");
    }
  }

  void Downloader::Cancel()
  {
    boost::lock_guard<boost::mutex> lock(LockStop);
    Stop = true;
  }

  TransferState Downloader::Wait(const unsigned ms)
  {
    boost::unique_lock<boost::mutex> lock(LockResult);

    typedef boost::chrono::system_clock time;
    time::time_point te = time::now() + boost::chrono::milliseconds(ms);

    while (!Done)
    {
      if (CondDone.wait_for(lock,  te - time::now()) == boost::cv_status::timeout)
      {
        return STATE_TIMEOUT;
      }
    }

    if (Cancelled())
    {
      return STATE_CANCELLED;
    }
    else if (Result)
    {
      return STATE_SUCCESS;
    }

    return STATE_ERROR;
  }

  std::string Downloader::Error() const
  {
    return SrcError;
  }

  boost::uint64_t Downloader::Size() const
  {
    return SrcSize;
  }

  bool Downloader::Cancelled() const
  {
    boost::lock_guard<boost::mutex> lock(LockStop);
    return Stop;
  }

  void Downloader::Receive(const void *buffer, size_t size, size_t nmemb)
  {
    Queue::Chunk chunk(size * nmemb);
    chunk.assign((char*)buffer, (char*)buffer + chunk.size());
    Q.Push(chunk);

    if (Obs)
    {
      Obs->UpdateProgress(chunk.size());
    }
  }

  void* Downloader::ThreadFunc(void *data)
  {
    Downloader* dl = static_cast<Downloader*>(data);

    bool res = dl->Src.Run(*dl, dl->SrcError);

    boost::lock_guard<boost::mutex> lock(dl->LockResult);
    dl->Result = res;
    dl->Done = true;

    dl->CondDone.notify_one();

    return 0;
  }
}