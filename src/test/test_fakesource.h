#pragma once

#include "../source.h"

#include <boost/thread/thread.hpp>

namespace FileTransfer
{
  namespace Test
  {
    class FakeSource : public Source
    {
    public:
      FakeSource(const std::string& data, unsigned delay, unsigned nloops = 1, bool error = false)
        : Data(data)
        , Delay(delay)
        , NLoops(nloops)
        , Error(error)
      {
      }

      virtual bool Run(Receiver& rv, std::string& strError)
      {
        for (unsigned i = 0; !rv.Cancelled() && (i < NLoops); ++i)
        {
          boost::this_thread::sleep_for(boost::chrono::milliseconds(Delay));
          rv.Receive((void*)Data.data(), 1, Data.size());
        }
        strError = (Error ? "ERROR" : "OK");
        return !Error;
      }

      virtual boost::uint64_t GetSize()
      {
        return Data.size() * NLoops;
      }

    private:
      const std::string Data;
      const unsigned Delay;
      const unsigned NLoops;
      const bool Error;
    };
  }
}
