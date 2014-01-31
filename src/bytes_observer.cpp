#include "bytes_observer.h"

#include <boost/thread/lock_guard.hpp>

namespace FileTransfer
{
  BytesObserver::BytesObserver(ProgressCalculator& progressCalculator)
    : ProgressCalculatorObj(progressCalculator)
    , Bytes(0)
  {
  }

  void BytesObserver::UpdateProgress(const size_t bytes)
  {
    {
      boost::lock_guard<boost::mutex> lock(LockBytes);
      Bytes += bytes;
    }
    ProgressCalculatorObj.Calculate();
  }

  boost::uint64_t BytesObserver::GetBytes() const
  {
    boost::lock_guard<boost::mutex> lock(LockBytes);
    return Bytes;
  }
}