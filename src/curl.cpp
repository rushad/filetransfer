#include "curl.h"

namespace FileTransfer
{
  CurlSource::CurlSource(const std::string& url)
    : Url(url)
  {
  }

  bool CurlSource::Run(Receiver& rv)
  {
    Curl = curl_easy_init();

    curl_easy_setopt(Curl, CURLOPT_URL, Url.c_str());
    curl_easy_setopt(Curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(Curl, CURLOPT_WRITEFUNCTION, WriteFunc);
    curl_easy_setopt(Curl, CURLOPT_WRITEDATA, &rv);

    CURLcode res = curl_easy_perform(Curl);

    return (res == CURLE_OK);
//      throw std::exception(curl_easy_strerror(res));
  }

  size_t CurlSource::WriteFunc(void* buffer, size_t size, size_t nmemb, void* data)
  {
    Receiver* rv = static_cast<Receiver*>(data);

    if (rv->Cancelled())
      return 0;

    rv->Receive(buffer, size, nmemb);

    return size * nmemb;
  }

  void CurlTarget::Run(Transmitter& tr)
  {
    Curl = curl_easy_init();

    curl_easy_setopt(Curl, CURLOPT_READFUNCTION, ReadFunc);
    curl_easy_setopt(Curl, CURLOPT_WRITEDATA, &tr);

    CURLcode res = curl_easy_perform(Curl);

    if (res != CURLE_OK)
      throw std::exception(curl_easy_strerror(res));
  }

  size_t CurlTarget::ReadFunc(void* buffer, size_t size, size_t nmemb, void* data)
  {
    Transmitter* tr = static_cast<Transmitter*>(data);
    size_t l = tr->Transmit(buffer, size, nmemb);
    if (tr->Cancelled())
      return CURL_READFUNC_ABORT;
    return l;
  }

}
