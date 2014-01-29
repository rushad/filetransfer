#pragma once

#include "../target.h"
#include "../transmitter.h"

#include <boost/shared_ptr.hpp>

#include <string>

namespace FileTransfer
{
  namespace Test
  {
    class FakeTarget : public Target
    {
    public:
      FakeTarget(bool error = false)
        : Error(error)
      {
      }

      virtual bool Run(Transmitter& tr, std::string& strError)
      {
        char buffer[100];
        size_t size;
        while (!tr.Cancelled() && (size = tr.Transmit(buffer, sizeof(buffer), 1)))
          Data += std::string(buffer, size);
        
        strError = (Error ? "ERROR" : "OK");

        return !Error;
      }

      std::string GetData() const
      {
        return Data;
      }

    typedef boost::shared_ptr<FakeTarget> Ptr;

    private:
      const bool Error;
      std::string Data;
    };

  }
}