#pragma once

#include <pthread.h>

namespace FileTransfer
{
  class Thread
  {
  public:
    typedef void*(*Function)(void*);

    Thread(Function func, void* data)
    {
      int r = pthread_create(&ThreadId, 0, func, data);
      switch(r)
      {
      case 0:
        break;
      case EAGAIN:
        throw std::exception("Thread(): EAGAIN: Insufficient resources to create thread");
      case EINVAL:
        throw std::exception("Thread(): EINVAL: Invalid settings in attr");
      case EPERM:
        throw std::exception("Thread(): EINVAL: No permission to set the scheduling policy and parameters");
      default:
        throw std::exception("Thread(): Unknown error");
      }
    }
    ~Thread()
    {
      pthread_join(ThreadId, 0);
    }

  private:
    pthread_t ThreadId;
  };
}