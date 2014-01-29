#pragma once

#include "../observer.h"
#include "../queue.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>

#include <string>

namespace FileTransfer
{
  namespace Test
  {
    using boost::posix_time::ptime;
    using boost::posix_time::millisec;

    const size_t CHUNK_SIZE     = 100;
    const unsigned CHUNK_DELAY  = 2;
    const unsigned CHUNK_NUMBER = 10;

    static ptime CurrentTime()
    {
      return boost::posix_time::microsec_clock::universal_time();
    }

    static void usleep(unsigned ms)
    {
      boost::this_thread::sleep_for(boost::chrono::milliseconds(ms));
    }

    static Queue::Chunk MakeChunk(const std::string& str)
    {
      Queue::Chunk chunk(str.size());
      chunk.assign(str.data(), str.data() + str.size());
      return chunk;
    }

    static Queue::Chunk MakeChunk(const Queue::Chunk& chunk1, const Queue::Chunk& chunk2)
    {
      Queue::Chunk chunk(chunk1);
      chunk.insert(chunk.end(), chunk2.begin(), chunk2.end());
      return chunk;
    }

    class FakeObserver : public Observer
    {
    public:
      virtual void UpdateProgress(const unsigned progress)
      {
        Progress = progress;
      }

      unsigned Progress;
    };
  }
}