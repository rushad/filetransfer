#pragma once

#include "transmitter.h"

namespace FileTransfer
{
  class Target
  {
  public:
    virtual ~Target() {}
    virtual bool Run(Transmitter& tr) = 0;
  };

}