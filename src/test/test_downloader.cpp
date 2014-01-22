#include "../downloader.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>

#include <gtest/gtest.h>

namespace FileTransfer
{
  namespace Test
  {
    class TestDownloader : public ::testing::Test
    {
    protected:
      static Queue::Chunk MakeChunk(const std::string& str)
      {
        Queue::Chunk chunk(str.size());
        chunk.assign(str.data(), str.data() + str.size());
        return chunk;
      }
      static void usleep(unsigned ms)
      {
        boost::this_thread::sleep_for(boost::chrono::milliseconds(ms));
      }
    };

    class FakeSource : public Source
    {
    public:
      FakeSource(const std::string& data, unsigned delay, unsigned nloops = 1)
        : Data(data)
        , Delay(delay)
        , NLoops(nloops)
      {
      }

      virtual bool Run(Receiver& rv)
      {
        for (unsigned i = 0; !rv.Cancelled() && (i < NLoops); ++i)
        {
          boost::this_thread::sleep_for(boost::chrono::milliseconds(Delay));
          rv.Receive((void*)Data.data(), 1, Data.size());
        }
        return !rv.Cancelled();
      }

    private:
      const std::string Data;
      const unsigned Delay;
      const unsigned NLoops;
    };

    TEST_F(TestDownloader, DownloaderShouldStartThread)
    {
      const std::string data(100, '$');
      FakeSource src(data, 2);
      Queue q;

      Downloader dl(src, q);

      dl.Start();

      ASSERT_EQ(0, q.Size());
    }

    TEST_F(TestDownloader, DownloaderShouldPushDataToQueue)
    {
      const std::string data(100, '$');
      FakeSource src(data, 2);
      Queue q;

      Downloader dl(src, q);

      dl.Start();

      usleep(10);

      ASSERT_EQ(MakeChunk(data), q.Pop(100));
    }

    TEST_F(TestDownloader, CancelShouldStopDownloader)
    {
      const std::string data(100, '$');
      FakeSource src(data, 2, 100);
      Queue q;

      Downloader dl(src, q);

      dl.Start();
      usleep(2);
      dl.Cancel();

      usleep(10);

      ASSERT_EQ(MakeChunk(data), q.Pop(1000));
    }
  }
}
