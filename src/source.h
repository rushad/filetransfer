#pragma once

#include "receiver.h"

#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>

#include <string>

namespace FileTransfer
{
  class Source
  {
  public:
    virtual ~Source() {}
    virtual bool Run(Receiver& rv, std::string& strError) = 0;
    virtual boost::uint64_t GetSize() = 0;
    typedef boost::shared_ptr<Source> Ptr;
  };

}