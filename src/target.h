#pragma once

#include "transmitter.h"

namespace FileTransfer
{
  class Target
  {
  public:
    virtual ~Target() {}
    virtual void Run(Transmitter& tr) = 0;
  };

}