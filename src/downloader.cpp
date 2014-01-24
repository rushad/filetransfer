#include "downloader.h"

#include <pthread.h>

namespace FileTransfer
{
  Downloader::Downloader(Source& src, Queue& q)
    : ThreadId(pthread_self())
    , Src(src)
    , Q(q)
    , Downloaded(0)
    , Stop(false)
    , Result(false)
  {
  }

  Downloader::~Downloader()
  {
    if (!pthread_equal(ThreadId, pthread_self()))
      pthread_join(ThreadId, 0);
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

  bool Downloader::Wait()
  {
    pthread_join(ThreadId, 0);
    boost::lock_guard<boost::mutex> lock(LockResult);
    return Result;
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
    Downloaded += chunk.size();
  }

  void* Downloader::ThreadFunc(void *data)
  {
    Downloader* dl = static_cast<Downloader*>(data);
    bool res = dl->Src.Run(*dl);
    boost::lock_guard<boost::mutex> lock(dl->LockResult);
    dl->Result = res;
    return 0;
  }
}