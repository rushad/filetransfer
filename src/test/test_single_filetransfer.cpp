#include "test_common.h"
#include "test_fakesource.h"
#include "test_faketarget.h"

#include "../single_filetransfer.h"

#include <gtest/gtest.h>

#include <string>

namespace FileTransfer
{
  namespace Test
  {
    class TestSingleFileTransfer : public ::testing::Test
    {
    };

    TEST_F(TestSingleFileTransfer, ShouldTransferData)
    {
      const std::string data(CHUNK_SIZE, '$');
      Source::Ptr src(new FakeSource(data, CHUNK_DELAY, CHUNK_NUMBER));
      Target::Ptr trg(new FakeTarget);

      SingleFileTransfer ft(src, trg);
      ft.Start();
      ft.Wait();

      ASSERT_EQ(CHUNK_SIZE * CHUNK_NUMBER, (static_cast<FakeTarget*>(trg.get()))->Data.size());
    }

    TEST_F(TestSingleFileTransfer, StartShouldNotWait)
    {
      const std::string data(CHUNK_SIZE, '$');
      Source::Ptr src(new FakeSource(data, CHUNK_DELAY, CHUNK_NUMBER));
      Target::Ptr trg(new FakeTarget);

      ptime t1(CurrentTime());
      SingleFileTransfer ft(src, trg);
      ft.Start();
      ptime t2(CurrentTime());

      ASSERT_LT(t2, t1 + millisec(CHUNK_DELAY * CHUNK_NUMBER));
    }

    TEST_F(TestSingleFileTransfer, WaitShouldWait)
    {
      const std::string data(CHUNK_SIZE, '$');
      Source::Ptr src(new FakeSource(data, CHUNK_DELAY, CHUNK_NUMBER));
      Target::Ptr trg(new FakeTarget);

      ptime t1(CurrentTime());
      SingleFileTransfer ft(src, trg);
      ft.Start();
      ft.Wait();
      ptime t2(CurrentTime());

      ASSERT_GT(t2, t1 + millisec(CHUNK_DELAY * CHUNK_NUMBER));
    }

    TEST_F(TestSingleFileTransfer, WaitShouldExitOnTimeout)
    {
      const std::string data(CHUNK_SIZE, '$');
      Source::Ptr src(new FakeSource(data, CHUNK_DELAY, CHUNK_NUMBER));
      Target::Ptr trg(new FakeTarget);

      ptime t1(CurrentTime());
      SingleFileTransfer ft(src, trg);
      ft.Start();
      ft.Wait(CHUNK_DELAY * CHUNK_NUMBER / 2);
      ptime t2(CurrentTime());

      ASSERT_LT(t2, t1 + millisec(CHUNK_DELAY * CHUNK_NUMBER));
    }

    TEST_F(TestSingleFileTransfer, ExecShouldWait)
    {
      const std::string data(CHUNK_SIZE, '$');
      Source::Ptr src(new FakeSource(data, CHUNK_DELAY, CHUNK_NUMBER));
      Target::Ptr trg(new FakeTarget);

      ptime t1(CurrentTime());
      SingleFileTransfer ft(src, trg);
      ft.Exec();
      ptime t2(CurrentTime());

      ASSERT_GT(t2, t1 + millisec(CHUNK_DELAY * CHUNK_NUMBER));
    }

    TEST_F(TestSingleFileTransfer, ExecShouldExitOnTimeout)
    {
      const std::string data(CHUNK_SIZE, '$');
      Source::Ptr src(new FakeSource(data, CHUNK_DELAY, CHUNK_NUMBER));
      Target::Ptr trg(new FakeTarget);

      ptime t1(CurrentTime());
      SingleFileTransfer ft(src, trg);
      ft.Exec(CHUNK_DELAY * CHUNK_NUMBER / 2);
      ptime t2(CurrentTime());

      ASSERT_LT(t2, t1 + millisec(CHUNK_DELAY * CHUNK_NUMBER));
    }

    TEST_F(TestSingleFileTransfer, CancelShouldCancel)
    {
      const std::string data(CHUNK_SIZE, '$');
      Source::Ptr src(new FakeSource(data, CHUNK_DELAY, CHUNK_NUMBER*1000000));
      Target::Ptr trg(new FakeTarget);

      ptime t1(CurrentTime());
      SingleFileTransfer ft(src, trg);
      ft.Start();
      usleep(CHUNK_DELAY * CHUNK_NUMBER / 2);
      ft.Cancel();
      ptime t2(CurrentTime());

      ASSERT_LT(t2, t1 + millisec(CHUNK_DELAY * CHUNK_NUMBER));
    }

    TEST_F(TestSingleFileTransfer, SecondWaitShouldContinueWaiting)
    {
      const std::string data(CHUNK_SIZE, '$');
      Source::Ptr src(new FakeSource(data, CHUNK_DELAY, CHUNK_NUMBER));
      FakeTarget::Ptr trg(new FakeTarget);

      SingleFileTransfer ft(src, trg);
      ft.Start();
      ft.Wait(CHUNK_DELAY * CHUNK_NUMBER / 2);
      ft.Wait();

      ASSERT_EQ(CHUNK_SIZE * CHUNK_NUMBER, trg.get()->Data.size());
    }
  }
}