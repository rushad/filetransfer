#pragma once

namespace FileTransfer
{
  class Observer
  {
  public:
    virtual ~Observer() {}
    virtual void UpdateProgress(const unsigned progress) = 0;
  };
}