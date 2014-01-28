#include "../downloader.h"
#include "../filetransfer.h"
#include "../source.h"
#include "../uploader.h"

#include <gtest/gtest.h>

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

    class TestFileTransfer : public ::testing::Test
    {
    };

    class TestUploader : public ::testing::Test
    {
    protected:
      static Queue::Chunk MakeChunk(const std::string& str)
      {
        Queue::Chunk chunk(str.size());
        chunk.assign(str.data(), str.data() + str.size());
        return chunk;
      }
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

    class FakeTarget : public Target
    {
    public:
      FakeTarget(bool error = false)
        : Error(error)
      {
      }

      virtual bool Run(Transmitter& tr, std::string& strError)
      {
        char buffer[100];
        size_t size;
        while (!tr.Cancelled() && (size = tr.Transmit(buffer, sizeof(buffer), 1)))
          Data += std::string(buffer, size);
        
        strError = (Error ? "ERROR" : "OK");

        return !Error;
      }

      std::string GetData() const
      {
        return Data;
      }

    private:
      const bool Error;
      std::string Data;
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

    TEST_F(TestFileTransfer, StartShouldNotWait)
    {
      const std::string data(CS, '$');
      FakeSource src(data, MS, NL);
      FakeTarget trg(true);

      ptime t1(CurrentTime());
      FileTransfer ft(&src, &trg);
      ft.Start();
      ptime t2(CurrentTime());

      ASSERT_LT(t2, t1 + millisec(MS * NL));
    }

    TEST_F(TestFileTransfer, WaitShouldWait)
    {
      const std::string data(CS, '$');
      FakeSource src(data, MS, NL);
      FakeTarget trg(true);

      ptime t1(CurrentTime());
      FileTransfer ft(&src, &trg);
      ft.Start();
      ft.Wait();
      ptime t2(CurrentTime());

      ASSERT_GT(t2, t1 + millisec(MS * NL));
    }

    TEST_F(TestFileTransfer, WaitShouldExitOnTimeout)
    {
      const std::string data(CS, '$');
      FakeSource src(data, MS, NL);
      FakeTarget trg(true);

      ptime t1(CurrentTime());
      FileTransfer ft(&src, &trg);
      ft.Start();
      ft.Wait(MS * NL / 2);
      ptime t2(CurrentTime());

      ASSERT_LT(t2, t1 + millisec(MS * NL));
    }

    TEST_F(TestFileTransfer, ExecShouldWait)
    {
      const std::string data(CS, '$');
      FakeSource src(data, MS, NL);
      FakeTarget trg(true);

      ptime t1(CurrentTime());
      FileTransfer ft(&src, &trg);
      ft.Exec();
      ptime t2(CurrentTime());

      ASSERT_GT(t2, t1 + millisec(MS * NL));
    }

    TEST_F(TestFileTransfer, ExecShouldExitOnTimeout)
    {
      const std::string data(CS, '$');
      FakeSource src(data, MS, NL);
      FakeTarget trg(true);

      ptime t1(CurrentTime());
      FileTransfer ft(&src, &trg);
      ft.Exec(MS * NL / 2);
      ptime t2(CurrentTime());

      ASSERT_LT(t2, t1 + millisec(MS * NL));
    }

    TEST_F(TestFileTransfer, CancelShouldCancel)
    {
      const std::string data(CS, '$');
      FakeSource src(data, MS, NL);
      FakeTarget trg(true);

      ptime t1(CurrentTime());
      FileTransfer ft(&src, &trg);
      ft.Start();
      usleep(MS * NL / 2);
      ft.Cancel();
      ptime t2(CurrentTime());

      ASSERT_LT(t2, t1 + millisec(MS * NL));
    }
  }
}
