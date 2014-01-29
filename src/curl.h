#pragma once

#include "receiver.h"
#include "source.h"
#include "target.h"

#include <curl/curl.h>

#include <boost/noncopyable.hpp>

#include <string>

namespace FileTransfer
{
  class CurlObj : private boost::noncopyable
  {
  public:
    CurlObj()
      : Obj(curl_easy_init())
    {
    }

    ~CurlObj()
    {
      curl_easy_cleanup(Obj);
    }

    CURL* operator()() const
    {
      return Obj;
    }

  private:
    CURL* Obj;
  };

  class CurlSource : public Source
  {
  public:
    CurlSource(const std::string& url);
    virtual bool Run(Receiver& wr, std::string& strError);
    virtual boost::uint64_t GetSize();

  private:
    static size_t WriteFunc(void* buffer, size_t size, size_t nmemb, void* data);
    static size_t WriteFuncHEAD(void* buffer, size_t size, size_t nmemb, void* data);

    const std::string Url;
  };

  class CurlTarget : public Target
  {
  public:
    CurlTarget(const std::string& url);
    virtual bool Run(Transmitter& tr, std::string& strError);

  private:
    static size_t ReadFunc(void* buffer, size_t size, size_t nmemb, void* data);

    const std::string Url;
  };

}