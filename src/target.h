#pragma once

#include "transmitter.h"

#include <string>

namespace FileTransfer
{
  class Target
  {
  public:
    virtual ~Target() {}
    virtual bool Run(Transmitter& tr, std::string& strError) = 0;
  };

}