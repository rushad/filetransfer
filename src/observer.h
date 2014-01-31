#pragma once

#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>

namespace FileTransfer
{
  class Observer
  {
  public:
    virtual ~Observer() {}
    virtual void UpdateProgress(const unsigned downloadProgress, const unsigned uploadProgress) = 0;
  };
}