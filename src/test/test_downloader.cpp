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

    TEST_F(TestDownloader, SizeShouldReturnSourceSize)
    {
      const std::string data(CHUNK_SIZE, '$');
      FakeSource src(data, CHUNK_DELAY);
      Queue q;

      Downloader dl(src, q);

      ASSERT_EQ(CHUNK_SIZE, dl.Size());
    }

    TEST_F(TestDownloader, DownloaderShouldStartThread)
    {
      const std::string data(CHUNK_SIZE, '$');
      FakeSource src(data, CHUNK_DELAY);
      Queue q;

      Downloader dl(src, q);

      dl.Start();

      ASSERT_EQ(0, q.Size());
    }

    TEST_F(TestDownloader, DownloaderShouldPushDataToQueue)
    {
      const std::string data(CHUNK_SIZE, '$');
      FakeSource src(data, CHUNK_DELAY);
      Queue q;

      Downloader dl(src, q);

      dl.Start();
      dl.Wait();

      ASSERT_EQ(MakeChunk(data), q.Pop(CHUNK_SIZE));
    }

    TEST_F(TestDownloader, CancelShouldStopDownloader)
    {
      const std::string data(CHUNK_SIZE, '$');
      FakeSource src(data, CHUNK_DELAY, CHUNK_NUMBER);
      Queue q;

      Downloader dl(src, q);

      dl.Start();
      usleep(CHUNK_DELAY);
      dl.Cancel();

      ASSERT_EQ(MakeChunk(data), q.Pop(CHUNK_SIZE * CHUNK_NUMBER));
    }

    TEST_F(TestDownloader, CheckSourceGetSize)
    {
      const std::string data(CHUNK_SIZE, '$');
      FakeSource src(data, CHUNK_DELAY, CHUNK_NUMBER);

      ASSERT_EQ(CHUNK_SIZE*CHUNK_NUMBER, src.GetSize());
    }

    TEST_F(TestDownloader, WaitShouldWait)
    {
      const std::string data(CHUNK_SIZE, '$');
      FakeSource src(data, CHUNK_DELAY, CHUNK_NUMBER);
      Queue q;

      Downloader dl(src, q);

      ptime t1(CurrentTime());
      dl.Start();
      dl.Wait();
      ptime t2(CurrentTime());

      ASSERT_GE(t2, t1 + millisec(CHUNK_DELAY * CHUNK_NUMBER) );
    }

    TEST_F(TestDownloader, WaitShouldReturnSuccessOnSuccess)
    {
      const std::string data(CHUNK_SIZE, '$');
      FakeSource src(data, CHUNK_DELAY, CHUNK_NUMBER);
      Queue q;

      Downloader dl(src, q);

      dl.Start();
      ASSERT_EQ(STATE_SUCCESS, dl.Wait());
    }

    TEST_F(TestDownloader, WaitShouldReturnCancelledWhenCancelled)
    {
      const std::string data(CHUNK_SIZE, '$');
      FakeSource src(data, CHUNK_DELAY, CHUNK_NUMBER);
      Queue q;

      Downloader dl(src, q);

      dl.Start();
      dl.Cancel();

      ASSERT_EQ(STATE_CANCELLED, dl.Wait());
    }

    TEST_F(TestDownloader, WaitShouldReturnTimeoutOnTimeout)
    {
      const std::string data(CHUNK_SIZE, '$');
      FakeSource src(data, CHUNK_DELAY, CHUNK_NUMBER);
      Queue q;

      Downloader dl(src, q);

      dl.Start();

      ASSERT_EQ(STATE_TIMEOUT, dl.Wait(CHUNK_DELAY * CHUNK_NUMBER / 2));
    }

    TEST_F(TestDownloader, WaitShouldReturnErrorOnError)
    {
      const std::string data(CHUNK_SIZE, '$');
      FakeSource src(data, CHUNK_DELAY, CHUNK_NUMBER, true);
      Queue q;

      Downloader dl(src, q);

      dl.Start();

      ASSERT_EQ(STATE_ERROR, dl.Wait());
    }

    TEST_F(TestDownloader, WaitShouldExitOnTimeout)
    {
      const std::string data(CHUNK_SIZE, '$');
      FakeSource src(data, CHUNK_DELAY, CHUNK_NUMBER);
      Queue q;

      Downloader dl(src, q);

      ptime t1(CurrentTime());
      dl.Start();
      dl.Wait(CHUNK_DELAY * CHUNK_NUMBER / 2);
      ptime t2(CurrentTime());

      ASSERT_LT(t2, t1 + millisec(CHUNK_DELAY * CHUNK_NUMBER));
    }

    TEST_F(TestDownloader, ErrorShouldReturnSourceError)
    {
      const std::string data(CHUNK_SIZE, '$');
      FakeSource src(data, CHUNK_DELAY, CHUNK_NUMBER, true);
      Queue q;

      Downloader dl(src, q);

      dl.Start();
      dl.Wait();

      ASSERT_EQ("ERROR", dl.Error());
    }

    TEST_F(TestDownloader, ObserverShouldGetProgress)
    {
      const std::string data(CHUNK_SIZE, '$');
      FakeSource src(data, CHUNK_DELAY, 1);
      FakeObserver obs;
      Queue q;

      Downloader dl(src, q, &obs);

      dl.Start();
      dl.Wait();

      ASSERT_EQ(100, obs.Progress);
    }
  }
}
