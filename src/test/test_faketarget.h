#pragma once

#include "../target.h"

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

    private:
      const bool Error;
      std::string Data;
    };

  }
}