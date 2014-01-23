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
    curl_easy_setopt(Curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(Curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(Curl, CURLOPT_WRITEFUNCTION, WriteFunc);
    curl_easy_setopt(Curl, CURLOPT_WRITEDATA, &rv);

    CURLcode res = curl_easy_perform(Curl);

    curl_easy_cleanup(Curl);

    return (res == CURLE_OK);
  }

  boost::uint64_t CurlSource::GetSize()
  {
    Curl = curl_easy_init();

    curl_easy_setopt(Curl, CURLOPT_URL, Url.c_str());
    curl_easy_setopt(Curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(Curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(Curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(Curl, CURLOPT_HEADER, 1);
    curl_easy_setopt(Curl, CURLOPT_NOBODY, 1);
    curl_easy_setopt(Curl, CURLOPT_WRITEFUNCTION, WriteFuncHEAD);
    
    CURLcode res = curl_easy_perform(Curl);

    curl_easy_cleanup(Curl);

    if (res != CURLE_OK)
      return -1;

    double size;
    curl_easy_getinfo(Curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &size);

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

  bool CurlTarget::Run(Transmitter& tr)
  {
    Curl = curl_easy_init();

    curl_easy_setopt(Curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(Curl, CURLOPT_PUT, 1L);
    curl_easy_setopt(Curl, CURLOPT_URL, Url.c_str());
    curl_easy_setopt(Curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(Curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(Curl, CURLOPT_READFUNCTION, ReadFunc);
    curl_easy_setopt(Curl, CURLOPT_READDATA, &tr);

    CURLcode res = curl_easy_perform(Curl);

    curl_easy_cleanup(Curl);

    return (res == CURLE_OK);
  }

  size_t CurlTarget::ReadFunc(void* buffer, size_t size, size_t nmemb, void* data)
  {
    Transmitter* tr = static_cast<Transmitter*>(data);
    size_t l = tr->Transmit(buffer, size, nmemb);
//    std::cout << std::string((char*)buffer, l) << std::endl;

    if (tr->Cancelled())
      return CURL_READFUNC_ABORT;

    return l;
  }

}
