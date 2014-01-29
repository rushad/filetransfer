#pragma once

#include "../queue.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>

namespace FileTransfer
{
  namespace Test
  {
    using boost::posix_time::ptime;
    using boost::posix_time::millisec;

    const size_t CS   = 100;
    const unsigned MS = 2;
    const unsigned NL = 10;

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
  }
}