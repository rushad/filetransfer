#pragma once

#include "transmitter.h"

#include <boost/shared_ptr.hpp>

#include <string>

namespace FileTransfer
{
  class Target
  {
  public:
    virtual ~Target() {}
    virtual bool Run(Transmitter& tr, std::string& strError) = 0;
    typedef boost::shared_ptr<Target> Ptr;
  };

}