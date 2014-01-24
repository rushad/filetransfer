#include "../downloader.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>

#include <gtest/gtest.h>

namespace FileTransfer
{
  namespace Test
  {
    using boost::posix_time::ptime;
    using boost::posix_time::millisec;
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

      static ptime CurrentTime()
      {
        return boost::posix_time::microsec_clock::universal_time();
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

      virtual boost::uint64_t GetSize()
      {
        return Data.size() * NLoops;
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

    TEST_F(TestDownloader, CheckSourceGetSize)
    {
      const std::string data(100, '$');
      FakeSource src(data, 2, 100);

      ASSERT_EQ(100*100, src.GetSize());
    }

    TEST_F(TestDownloader, WaitShouldWait)
    {
      const std::string data(100, '$');
      FakeSource src(data, 2, 100);
      Queue q;

      Downloader dl(src, q);

      ptime t1(CurrentTime());
      dl.Start();
      dl.Wait();
      ptime t2(CurrentTime());

      ASSERT_GE(t2, t1 + millisec(2 * 100) );
    }

    TEST_F(TestDownloader, WaitShouldReturnTrueOnSuccess)
    {
      const std::string data(100, '$');
      FakeSource src(data, 2, 100);
      Queue q;

      Downloader dl(src, q);

      ptime t1(CurrentTime());
      dl.Start();
      ASSERT_TRUE(dl.Wait());
    }

    TEST_F(TestDownloader, WaitShouldReturnFalseWhenCancelled)
    {
      const std::string data(100, '$');
      FakeSource src(data, 2, 100);
      Queue q;

      Downloader dl(src, q);

      ptime t1(CurrentTime());
//      dl.Start();
      dl.Cancel();
      ASSERT_FALSE(dl.Wait());
    }
  }
}
