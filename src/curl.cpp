#include "curl.h"

namespace FileTransfer
{
  CurlSource::CurlSource(const std::string& url)
    : Url(url)
  {
  }

  bool CurlSource::Run(Receiver& rv, std::string& strError)
  {
    CurlObj curl;

    curl_easy_setopt(curl(), CURLOPT_URL, Url.c_str());
    curl_easy_setopt(curl(), CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl(), CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl(), CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl(), CURLOPT_WRITEFUNCTION, WriteFunc);
    curl_easy_setopt(curl(), CURLOPT_WRITEDATA, &rv);

    CURLcode res = curl_easy_perform(curl());

    strError = curl_easy_strerror(res);

    return (res == CURLE_OK);
  }

  boost::uint64_t CurlSource::GetSize()
  {
    CurlObj curl;

    curl_easy_setopt(curl(), CURLOPT_URL, Url.c_str());
    curl_easy_setopt(curl(), CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl(), CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl(), CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl(), CURLOPT_HEADER, 1);
    curl_easy_setopt(curl(), CURLOPT_NOBODY, 1);
    curl_easy_setopt(curl(), CURLOPT_WRITEFUNCTION, WriteFuncHEAD);
    
    CURLcode res = curl_easy_perform(curl());

    if (res != CURLE_OK)
      return -1;

    double size = 0;
    curl_easy_getinfo(curl(), CURLINFO_CONTENT_LENGTH_DOWNLOAD, &size);

    return static_cast<boost::uint64_t>(size);
  }

  size_t CurlSource::WriteFunc(void* buffer, size_t size, size_t nmemb, void* data)
  {
    Receiver* rv = static_cast<Receiver*>(data);

    if (rv->Cancelled())
      return 0;

    rv->Receive(buffer, size, nmemb);

    return size * nmemb;
  }

  size_t CurlSource::WriteFuncHEAD(void* buffer, size_t size, size_t nmemb, void* data)
  {
    return size * nmemb;
  }

  CurlTarget::CurlTarget(const std::string& url)
    : Url(url)
  {
  }

  bool CurlTarget::Run(Transmitter& tr, std::string& strError)
  {
    CurlObj curl;

    curl_easy_setopt(curl(), CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl(), CURLOPT_PUT, 1L);
    curl_easy_setopt(curl(), CURLOPT_URL, Url.c_str());
    curl_easy_setopt(curl(), CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl(), CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl(), CURLOPT_READFUNCTION, ReadFunc);
    curl_easy_setopt(curl(), CURLOPT_READDATA, &tr);

    CURLcode res = curl_easy_perform(curl());

    strError = curl_easy_strerror(res);

    return (res == CURLE_OK);
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
