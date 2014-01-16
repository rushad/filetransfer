#include "curl.h"

#include <curl/curl.h>

namespace FileTransfer
{
  void CurlSource::Run(Receiver& wr)
  {
/*    Curl = curl_easy_init();
    curl_easy_setopt(Curl, CURLOPT_WRITEFUNCTION, WriteFunc);
    curl_easy_setopt(Curl, CURLOPT_WRITEDATA, &wr);
    res = curl_easy_perform(Curl);*/
  }

  size_t CurlSource::WriteFunc(void* buffer, size_t size, size_t nmemb, void* data)
  {
    Receiver* rv = static_cast<Receiver*>(data);
    rv->Receive(buffer, size, nmemb);
    return size * nmemb;
  }
}