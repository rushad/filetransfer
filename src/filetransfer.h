#pragma once

#include <limits.h>

namespace FileTransfer
{
  const static unsigned WAIT_INFINITE = UINT_MAX;

  class FileTransfer
  {
  public:
    virtual void Start() = 0;
    virtual void Wait(const unsigned timeout) = 0;
    virtual void Cancel() = 0;
  };
}