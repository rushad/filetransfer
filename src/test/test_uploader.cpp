#include "test_common.h"
#include "test_fakesource.h"
#include "test_faketarget.h"

#include "../downloader.h"
#include "../filetransfer.h"
#include "../uploader.h"

#include <gtest/gtest.h>

#include <string>

namespace FileTransfer
{
  namespace Test
  {
    class TestUploader : public ::testing::Test
    {
    };

    TEST_F(TestUploader, UploaderShouldStartThread)
    {
      const std::string data(CHUNK_SIZE, '$');
      FakeTarget trg;
      Queue q;

      q.Push(MakeChunk(data));

      Uploader ul(trg, q, data.size());
      ul.Start();

      ASSERT_LT(trg.GetData().size(), data.size());
    }

    TEST_F(TestUploader, UploaderShouldPopDataFromQueue)
    {
      const std::string data(CHUNK_SIZE, '$');
      FakeTarget trg;
      Queue q;

      q.Push(MakeChunk(data));

      Uploader ul(trg, q, data.size());
      ul.Start();
      ul.Wait();

      ASSERT_EQ(data, trg.GetData());
    }

    TEST_F(TestUploader, UploaderShouldStopWhenDone)
    {
      const std::string data(CHUNK_SIZE, '$');
      FakeTarget trg;
      Queue q;

      Uploader ul(trg, q, data.size());
      ul.Start();

      q.Push(MakeChunk(data));
      usleep(CHUNK_DELAY);

      q.Push(MakeChunk(data));
      usleep(CHUNK_DELAY);

      ASSERT_EQ(data, trg.GetData());
    }

    TEST_F(TestUploader, CancelShouldStopUploader)
    {
      const std::string data(CHUNK_SIZE, '$');
      FakeTarget trg;
      Queue q;

      Uploader ul(trg, q, data.size() * 2);
      ul.Start();

      q.Push(MakeChunk(data));
      usleep(CHUNK_DELAY);

      ul.Cancel();

      q.Push(MakeChunk(data));
      usleep(CHUNK_DELAY);

      ASSERT_EQ(data, trg.GetData());
    }

    TEST_F(TestUploader, WaitShouldWait)
    {
      const std::string data(CHUNK_SIZE, '$');
      FakeSource src(data, CHUNK_DELAY, CHUNK_NUMBER);
      FakeTarget trg;
      Queue q;

      Downloader dl(src, q);
      Uploader ul(trg, q, dl.Size());

      dl.Start();
      ul.Start();

      ptime t1(CurrentTime());
      ul.Wait();
      ptime t2(CurrentTime());

      ASSERT_GE(t2, t1 + millisec(CHUNK_DELAY * CHUNK_NUMBER) );
    }

    TEST_F(TestUploader, WaitShouldReturnSuccessOnSuccess)
    {
      const std::string data(CHUNK_SIZE, '$');
      FakeSource src(data, CHUNK_DELAY, CHUNK_NUMBER);
      FakeTarget trg;
      Queue q;

      Downloader dl(src, q);
      Uploader ul(trg, q, dl.Size());

      dl.Start();
      ul.Start();

      ASSERT_EQ(STATE_SUCCESS, ul.Wait());
    }

    TEST_F(TestUploader, WaitShouldReturnCancelledWhenCancelled)
    {
      const std::string data(CHUNK_SIZE, '$');
      FakeSource src(data, CHUNK_DELAY, CHUNK_NUMBER);
      FakeTarget trg;
      Queue q;

      Downloader dl(src, q);
      Uploader ul(trg, q, dl.Size());

      dl.Start();
      ul.Start();
      ul.Cancel();

      ASSERT_EQ(STATE_CANCELLED, ul.Wait());
    }

    TEST_F(TestUploader, WaitShouldReturnTimeoutOnTimeout)
    {
      const std::string data(CHUNK_SIZE, '$');
      FakeSource src(data, CHUNK_DELAY, CHUNK_NUMBER);
      FakeTarget trg;
      Queue q;

      Downloader dl(src, q);
      Uploader ul(trg, q, dl.Size());

      dl.Start();
      ul.Start();

      ASSERT_EQ(STATE_TIMEOUT, ul.Wait(CHUNK_DELAY * CHUNK_NUMBER / 2));
    }

    TEST_F(TestUploader, WaitShouldReturnErrorOnError)
    {
      const std::string data(CHUNK_SIZE, '$');
      FakeSource src(data, CHUNK_DELAY, CHUNK_NUMBER);
      FakeTarget trg(true);
      Queue q;

      Downloader dl(src, q);
      Uploader ul(trg, q, dl.Size());

      dl.Start();
      ul.Start();

      ASSERT_EQ(STATE_ERROR, ul.Wait());
    }

    TEST_F(TestUploader, WaitShouldExitOnTimeout)
    {
      const std::string data(CHUNK_SIZE, '$');
      FakeSource src(data, CHUNK_DELAY, CHUNK_NUMBER);
      FakeTarget trg;
      Queue q;

      Downloader dl(src, q);
      Uploader ul(trg, q, dl.Size());

      dl.Start();

      ptime t1(CurrentTime());
      ul.Start();
      ul.Wait(CHUNK_DELAY * CHUNK_NUMBER / 2);
      ptime t2(CurrentTime());

      ASSERT_LT(t2, t1 + millisec(CHUNK_DELAY * CHUNK_NUMBER));
    }

    TEST_F(TestUploader, ErrorShouldReturnTargetError)
    {
      const std::string data(CHUNK_SIZE, '$');
      FakeSource src(data, CHUNK_DELAY, CHUNK_NUMBER);
      FakeTarget trg(true);
      Queue q;

      Downloader dl(src, q);
      Uploader ul(trg, q, dl.Size());

      dl.Start();
      ul.Start();
      ul.Wait();

      ASSERT_EQ("ERROR", ul.Error());
    }

    TEST_F(TestUploader, ObserverShouldGetProgress)
    {
      const std::string data(CHUNK_SIZE, '$');
      FakeSource src(data, CHUNK_DELAY, 1);
      FakeTarget trg;
      FakeObserver obs;
      Queue q;

      Downloader dl(src, q);
      Uploader ul(trg, q, dl.Size(), &obs);

      dl.Start();
      ul.Start();
      ul.Wait();

      ASSERT_EQ(100, obs.Progress);
    }
  }
}
