#pragma once

namespace FileTransfer
{
  class ProgressCalculator
  {
  public:
    virtual ~ProgressCalculator()
    {
    }

    virtual void Calculate() = 0;
  };
}