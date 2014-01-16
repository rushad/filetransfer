#pragma once

#include "receiver.h"

namespace FileTransfer
{
  class Source
  {
  public:
    virtual ~Source() {}
    virtual void Run(Receiver& wr) = 0;
  };

}