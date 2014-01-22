#pragma once

#include "receiver.h"

namespace FileTransfer
{
  class Source
  {
  public:
    virtual ~Source() {}
    virtual bool Run(Receiver& rv) = 0;
  };

}