#include "test_common.h"
#include "test_fakesource.h"

#include "../downloader.h"

#include <gtest/gtest.h>

namespace FileTransfer
{
  namespace Test
  {
    class TestDownloader : public ::testing::Test
    {
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
