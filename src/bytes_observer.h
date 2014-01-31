#pragma once

#include "filetransfer.h"
#include "progress_calculator.h"

#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

#include <vector>

namespace FileTransfer
{
  class BytesObserver
  {
  public:
    BytesObserver(ProgressCalculator& ft);

    void UpdateProgress(const size_t bytes);
    boost::uint64_t GetBytes() const;

    typedef boost::shared_ptr<BytesObserver> Ptr;

  private:
    ProgressCalculator& ProgressCalculatorObj;
    boost::uint64_t Bytes;
    mutable boost::mutex LockBytes;
  };
}