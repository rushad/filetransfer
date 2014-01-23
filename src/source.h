#pragma once

#include "receiver.h"

#include <boost/cstdint.hpp>

namespace FileTransfer
{
  class Source
  {
  public:
    virtual ~Source() {}
    virtual bool Run(Receiver& rv) = 0;
    virtual boost::uint64_t GetSize() = 0;
  };

}