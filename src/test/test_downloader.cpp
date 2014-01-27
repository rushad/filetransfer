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
      TestDownloader()
        : CS(100)
        , MS(2)
        , NL(10)
      {
      }

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

      const size_t CS;
      const unsigned MS;
      const unsigned NL;
    };

    class FakeSource : public Source
    {
    public:
      FakeSource(const std::string& data, unsigned delay, unsigned nloops = 1, bool error = false)
        : Data(data)
        , Delay(delay)
        , NLoops(nloops)
        , Error(error)
      {
      }

      virtual bool Run(Receiver& rv, std::string& strError)
      {
        for (unsigned i = 0; !rv.Cancelled() && (i < NLoops); ++i)
        {
          boost::this_thread::sleep_for(boost::chrono::milliseconds(Delay));
          rv.Receive((void*)Data.data(), 1, Data.size());
        }
        strError = (Error ? "ERROR" : "OK");
        return !Error;
      }

      virtual boost::uint64_t GetSize()
      {
        return Data.size() * NLoops;
      }

    private:
      const std::string Data;
      const unsigned Delay;
      const unsigned NLoops;
      const bool Error;
    };

    TEST_F(TestDownloader, DownloaderShouldStartThread)
    {
      const std::string data(CS, '$');
      FakeSource src(data, MS);
      Queue q;

      Downloader dl(src, q);

      dl.Start();

      ASSERT_EQ(0, q.Size());
    }

    TEST_F(TestDownloader, DownloaderShouldPushDataToQueue)
    {
      const std::string data(CS, '$');
      FakeSource src(data, MS);
      Queue q;

      Downloader dl(src, q);

      dl.Start();
      dl.Wait();

      ASSERT_EQ(MakeChunk(data), q.Pop(CS));
    }

    TEST_F(TestDownloader, CancelShouldStopDownloader)
    {
      const std::string data(CS, '$');
      FakeSource src(data, MS, NL);
      Queue q;

      Downloader dl(src, q);

      dl.Start();
      usleep(MS);
      dl.Cancel();

      ASSERT_EQ(MakeChunk(data), q.Pop(CS * NL));
    }

    TEST_F(TestDownloader, CheckSourceGetSize)
    {
      const std::string data(CS, '$');
      FakeSource src(data, MS, NL);

      ASSERT_EQ(CS*NL, src.GetSize());
    }

    TEST_F(TestDownloader, WaitShouldWait)
    {
      const std::string data(CS, '$');
      FakeSource src(data, MS, NL);
      Queue q;

      Downloader dl(src, q);

      ptime t1(CurrentTime());
      dl.Start();
      dl.Wait();
      ptime t2(CurrentTime());

      ASSERT_GE(t2, t1 + millisec(MS * NL) );
    }

    TEST_F(TestDownloader, WaitShouldReturnSuccessOnSuccess)
    {
      const std::string data(CS, '$');
      FakeSource src(data, MS, NL);
      Queue q;

      Downloader dl(src, q);

      dl.Start();
      ASSERT_EQ(STATE_SUCCESS, dl.Wait());
    }

    TEST_F(TestDownloader, WaitShouldReturnCancelledWhenCancelled)
    {
      const std::string data(CS, '$');
      FakeSource src(data, MS, NL);
      Queue q;

      Downloader dl(src, q);

      dl.Start();
      dl.Cancel();

      ASSERT_EQ(STATE_CANCELLED, dl.Wait());
    }

    TEST_F(TestDownloader, WaitShouldReturnTimeoutOnTimeout)
    {
      const std::string data(CS, '$');
      FakeSource src(data, MS, NL);
      Queue q;

      Downloader dl(src, q);

      dl.Start();

      ASSERT_EQ(STATE_TIMEOUT, dl.Wait(MS * NL / 2));
    }

    TEST_F(TestDownloader, WaitShouldReturnErrorOnError)
    {
      const std::string data(CS, '$');
      FakeSource src(data, MS, NL, true);
      Queue q;

      Downloader dl(src, q);

      dl.Start();

      ASSERT_EQ(STATE_ERROR, dl.Wait());
    }

    TEST_F(TestDownloader, WaitShouldExitOnTimeout)
    {
      const std::string data(CS, '$');
      FakeSource src(data, MS, NL);
      Queue q;

      Downloader dl(src, q);

      ptime t1(CurrentTime());
      dl.Start();
      dl.Wait(MS * NL / 2);
      ptime t2(CurrentTime());

      ASSERT_LT(t2, t1 + millisec(MS * NL));
    }

    TEST_F(TestDownloader, ErrorShouldReturnSourceError)
    {
      const std::string data(CS, '$');
      FakeSource src(data, MS, NL, true);
      Queue q;

      Downloader dl(src, q);

      dl.Start();
      dl.Wait();

      ASSERT_EQ("ERROR", dl.Error());
    }
  }
}
