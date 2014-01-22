#pragma once

#include "receiver.h"
#include "source.h"
#include "target.h"

#include <curl/curl.h>

#include <string>

namespace FileTransfer
{
  class CurlSource : public Source
  {
  public:
    CurlSource(const std::string& url);
    virtual bool Run(Receiver& wr);

  private:
    static size_t WriteFunc(void* buffer, size_t size, size_t nmemb, void* data);

    std::string Url;
    CURL* Curl;
  };

  class CurlTarget : public Target
  {
  public:
    virtual void Run(Transmitter& tr);

  private:
    static size_t ReadFunc(void* buffer, size_t size, size_t nmemb, void* data);

    CURL* Curl;
  };

}