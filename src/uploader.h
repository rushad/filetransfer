#pragma once

#include "bytes_observer.h"
#include "queue.h"
#include "target.h"
#include "thread.h"
#include "transfer.h"
#include "transmitter.h"

#include <pthread.h>

#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>

#include <string>
#include <vector>

namespace FileTransfer
{
  class Uploader : public Transmitter
  {
  public:
    Uploader(Target& trg, Queue& q, const boost::uint64_t length);
    void SetObserver(BytesObserver::Ptr obs);
    void Start();
    void Cancel();
    TransferState Wait(const unsigned ms = UINT_MAX);
    std::string Error() const;

    virtual bool Cancelled() const;
    virtual size_t Transmit(void *buffer, size_t size, size_t nmemb);

    typedef boost::shared_ptr<Uploader> Ptr;
    typedef std::vector<Ptr> Vector;

  private:
    static void* ThreadFunc(void *data);
    std::auto_ptr<Thread> ThreadObj;

    Target& Trg;
    Queue& Q;
    BytesObserver::Ptr Obs;
    const boost::uint64_t SrcSize;
    boost::uint64_t Uploaded;
 
    mutable boost::mutex LockStop;
    bool Stop;

    mutable boost::mutex LockResult;
    bool Result;
    bool Done;
    boost::condition_variable CondDone;

    std::string SrcError;
  };
}