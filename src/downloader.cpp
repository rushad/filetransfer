#include "downloader.h"

#include <iostream> // ****
#include <string> // ****

namespace FileTransfer
{
  Downloader::Downloader(Source& src, Queue& q)
    : Src(src)
    , Q(q)
  {
  }

  void Downloader::Start()
  {
    ThreadFunc(this);
    //pthread_create(Perform, this);
  }

  void Downloader::Receive(const void *buffer, size_t size, size_t nmemb)
  {
    // Q->AddChunk(...);
    std::cout << std::string((char*)buffer, size * nmemb) << std::endl;
  }

  void* Downloader::ThreadFunc(void *data)
  {
    Downloader* dl = static_cast<Downloader*>(data);
    dl->Src.Run(*dl);
    return 0;
  }
}