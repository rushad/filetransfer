#include "downloader.h"

#include <pthread.h>

namespace FileTransfer
{
  Downloader::Downloader(Source& src, Queue& q)
    : /*ThreadId(0)
    , */Src(src)
    , Q(q)
  {
  }

  void Downloader::Start()
  {
    ThreadFunc(this);

//    int r = pthread_create(Perform, this
  }

  void Downloader::Receive(const void *buffer, size_t size, size_t nmemb)
  {
    Queue::Chunk chunk(size * nmemb);
    chunk.assign((char*)buffer, (char*)buffer + chunk.size());
    Q.Push(chunk);
  }

  void* Downloader::ThreadFunc(void *data)
  {
    Downloader* dl = static_cast<Downloader*>(data);
    dl->Src.Run(*dl);
    return 0;
  }
}