#include "test_common.h"
#include "test_fakesource.h"
#include "test_faketarget.h"

#include "../downloader.h"
#include "../filetransfer.h"
#include "../uploader.h"

#include <gtest/gtest.h>

namespace FileTransfer
{
  namespace Test
  {
    class TestUploader : public ::testing::Test
    {
    };

    TEST_F(TestUploader, UploaderShouldStartThread)
    {
      const std::string data(CS, '$');
      FakeTarget trg;
      Queue q;

      q.Push(MakeChunk(data));

      Uploader ul(trg, q, data.size());
      ul.Start();

      ASSERT_LT(trg.GetData().size(), data.size());
    }

    TEST_F(TestUploader, UploaderShouldPopDataFromQueue)
    {
      const std::string data(CS, '$');
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
      const std::string data(CS, '$');
      FakeTarget trg;
      Queue q;

      Uploader ul(trg, q, data.size());
      ul.Start();

      q.Push(MakeChunk(data));
      usleep(MS);

      q.Push(MakeChunk(data));
      usleep(MS);

      ASSERT_EQ(data, trg.GetData());
    }

    TEST_F(TestUploader, CancelShouldStopUploader)
    {
      const std::string data(CS, '$');
      FakeTarget trg;
      Queue q;

      Uploader ul(trg, q, data.size() * 2);
      ul.Start();

      q.Push(MakeChunk(data));
      usleep(MS);

      ul.Cancel();

      q.Push(MakeChunk(data));
      usleep(MS);

      ASSERT_EQ(data, trg.GetData());
    }

    TEST_F(TestUploader, WaitShouldWait)
    {
      const std::string data(CS, '$');
      FakeSource src(data, MS, NL);
      FakeTarget trg;
      Queue q;

      Downloader dl(src, q);
      Uploader ul(trg, q, data.size() * NL);

      dl.Start();
      ul.Start();

      ptime t1(CurrentTime());
      ul.Wait();
      ptime t2(CurrentTime());

      ASSERT_GE(t2, t1 + millisec(MS * NL) );
    }

    TEST_F(TestUploader, WaitShouldReturnSuccessOnSuccess)
    {
      const std::string data(CS, '$');
      FakeSource src(data, MS, NL);
      FakeTarget trg;
      Queue q;

      Downloader dl(src, q);
      Uploader ul(trg, q, data.size() * NL);

      dl.Start();
      ul.Start();

      ASSERT_EQ(STATE_SUCCESS, ul.Wait());
    }

    TEST_F(TestUploader, WaitShouldReturnCancelledWhenCancelled)
    {
      const std::string data(CS, '$');
      FakeSource src(data, MS, NL);
      FakeTarget trg;
      Queue q;

      Downloader dl(src, q);
      Uploader ul(trg, q, data.size() * NL);

      dl.Start();
      ul.Start();
      ul.Cancel();

      ASSERT_EQ(STATE_CANCELLED, ul.Wait());
    }

    TEST_F(TestUploader, WaitShouldReturnTimeoutOnTimeout)
    {
      const std::string data(CS, '$');
      FakeSource src(data, MS, NL);
      FakeTarget trg;
      Queue q;

      Downloader dl(src, q);
      Uploader ul(trg, q, data.size() * NL);

      dl.Start();
      ul.Start();

      ASSERT_EQ(STATE_TIMEOUT, ul.Wait(MS * NL / 2));
    }

    TEST_F(TestUploader, WaitShouldReturnErrorOnError)
    {
      const std::string data(CS, '$');
      FakeSource src(data, MS, NL);
      FakeTarget trg(true);
      Queue q;

      Downloader dl(src, q);
      Uploader ul(trg, q, data.size() * NL);

      dl.Start();
      ul.Start();

      ASSERT_EQ(STATE_ERROR, ul.Wait());
    }

    TEST_F(TestUploader, WaitShouldExitOnTimeout)
    {
      const std::string data(CS, '$');
      FakeSource src(data, MS, NL);
      FakeTarget trg(true);
      Queue q;

      Downloader dl(src, q);
      Uploader ul(trg, q, data.size() * NL);

      dl.Start();

      ptime t1(CurrentTime());
      ul.Start();
      ul.Wait(MS * NL / 2);
      ptime t2(CurrentTime());

      ASSERT_LT(t2, t1 + millisec(MS * NL));
    }

    TEST_F(TestUploader, ErrorShouldReturnSourceError)
    {
      const std::string data(CS, '$');
      FakeSource src(data, MS, NL);
      FakeTarget trg(true);
      Queue q;

      Downloader dl(src, q);
      Uploader ul(trg, q, data.size() * NL);

      dl.Start();
      ul.Start();
      ul.Wait();

      ASSERT_EQ("ERROR", ul.Error());
    }
  }
}
