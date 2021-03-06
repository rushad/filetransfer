#include "uploader.h"

namespace FileTransfer
{
  Uploader::Uploader(Target& trg, Queue& q, const boost::uint64_t srcSize)
    : Trg(trg)
    , Q(q)
    , SrcSize(srcSize)
    , Uploaded(0)
    , Stop(false)
    , Result(false)
    , Done(false)
  {
  }

  void Uploader::SetObserver(BytesObserver::Ptr obs)
  {
    Obs = obs;
  }

  void Uploader::Start()
  {
    ThreadObj = std::auto_ptr<Thread>(new Thread(ThreadFunc, this));
  }

  void Uploader::Cancel()
  {
    boost::lock_guard<boost::mutex> lock(LockStop);
    Stop = true;
    Q.Cancel();
  }

  TransferState Uploader::Wait(const unsigned ms)
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

  std::string Uploader::Error() const
  {
    return SrcError;
  }

  bool Uploader::Cancelled() const
  {
    boost::lock_guard<boost::mutex> lock(LockStop);
    return Stop;
  }

  size_t Uploader::Transmit(void *buffer, size_t size, size_t nmemb)
  {
    if (Uploaded >= SrcSize)
      return 0;

    Queue::Chunk chunk = Q.Pop(size * nmemb);

    if (chunk.size() > 0)
      memcpy(buffer, &chunk[0], chunk.size());

    Uploaded += chunk.size();

    if (Obs)
    {
      Obs->UpdateProgress(chunk.size());
    }

    return chunk.size();
  }

  void* Uploader::ThreadFunc(void *data)
  {
    Uploader* ul = static_cast<Uploader*>(data);

    bool res = ul->Trg.Run(*ul, ul->SrcError);

    boost::lock_guard<boost::mutex> lock(ul->LockResult);
    ul->Result = res;
    ul->Done = true;
    
    ul->CondDone.notify_one();

    return 0;
  }
}