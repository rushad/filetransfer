#include "uploader.h"

#include <pthread.h>

namespace FileTransfer
{
  Uploader::Uploader(Target& trg, Queue& q, const boost::uint64_t length)
    : ThreadId(pthread_self())
    , Trg(trg)
    , Q(q)
    , Length(length)
    , Uploaded(0)
    , Stop(false)
    , Result(false)
  {
  }

  Uploader::~Uploader()
  {
    if (!pthread_equal(ThreadId, pthread_self()))
      pthread_join(ThreadId, 0);
  }

  void Uploader::Start()
  {
    int r = pthread_create(&ThreadId, 0, ThreadFunc, this);
    switch(r)
    {
    case 0:
      break;
    case EAGAIN:
      throw std::exception("Uploader::Start(): EAGAIN: Insufficient resources to create thread");
    case EINVAL:
      throw std::exception("Uploader::Start(): EINVAL: Invalid settings in attr");
    case EPERM:
      throw std::exception("Uploader::Start(): EINVAL: No permission to set the scheduling policy and parameters");
    default:
      throw std::exception("Uploader::Start(): Unknown error");
    }
  }

  void Uploader::Cancel()
  {
    boost::lock_guard<boost::mutex> lock(LockStop);
    Stop = true;
    Q.Cancel();
  }

  bool Uploader::Wait()
  {
    pthread_join(ThreadId, 0);
    boost::lock_guard<boost::mutex> lock(LockResult);
    return Result;
  }

  bool Uploader::Cancelled() const
  {
    boost::lock_guard<boost::mutex> lock(LockStop);
    return Stop;
  }

  size_t Uploader::Transmit(void *buffer, size_t size, size_t nmemb)
  {
    if (Uploaded >= Length)
      return 0;

    Queue::Chunk chunk = Q.Pop(size * nmemb);

    if (chunk.size() > 0)
      memcpy(buffer, &chunk[0], chunk.size());

    Uploaded += chunk.size();

    return chunk.size();
  }

  void* Uploader::ThreadFunc(void *data)
  {
    Uploader* ul = static_cast<Uploader*>(data);
    bool res = ul->Trg.Run(*ul);
    boost::lock_guard<boost::mutex> lock(ul->LockResult);
    ul->Result = res;
    return 0;
  }
}