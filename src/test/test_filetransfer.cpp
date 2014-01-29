#include "test_common.h"
#include "test_fakesource.h"
#include "test_faketarget.h"

#include "../filetransfer.h"

#include <gtest/gtest.h>

#include <string>

namespace FileTransfer
{
  namespace Test
  {
    class TestFileTransfer : public ::testing::Test
    {
    };

    TEST_F(TestFileTransfer, StartShouldNotWait)
    {
      const std::string data(CHUNK_SIZE, '$');
      Source::Ptr src(new FakeSource(data, CHUNK_DELAY, CHUNK_NUMBER));
      Target::Ptr trg(new FakeTarget);

      ptime t1(CurrentTime());
      FileTransfer ft(src, trg);
      ft.Start();
      ptime t2(CurrentTime());

      ASSERT_LT(t2, t1 + millisec(CHUNK_DELAY * CHUNK_NUMBER));
    }

    TEST_F(TestFileTransfer, WaitShouldWait)
    {
      const std::string data(CHUNK_SIZE, '$');
      Source::Ptr src(new FakeSource(data, CHUNK_DELAY, CHUNK_NUMBER));
      Target::Ptr trg(new FakeTarget);

      ptime t1(CurrentTime());
      FileTransfer ft(src, trg);
      ft.Start();
      ft.Wait();
      ptime t2(CurrentTime());

      ASSERT_GT(t2, t1 + millisec(CHUNK_DELAY * CHUNK_NUMBER));
    }

    TEST_F(TestFileTransfer, WaitShouldExitOnTimeout)
    {
      const std::string data(CHUNK_SIZE, '$');
      Source::Ptr src(new FakeSource(data, CHUNK_DELAY, CHUNK_NUMBER));
      Target::Ptr trg(new FakeTarget);

      ptime t1(CurrentTime());
      FileTransfer ft(src, trg);
      ft.Start();
      ft.Wait(CHUNK_DELAY * CHUNK_NUMBER / 2);
      ptime t2(CurrentTime());

      ASSERT_LT(t2, t1 + millisec(CHUNK_DELAY * CHUNK_NUMBER));
    }

    TEST_F(TestFileTransfer, ExecShouldWait)
    {
      const std::string data(CHUNK_SIZE, '$');
      Source::Ptr src(new FakeSource(data, CHUNK_DELAY, CHUNK_NUMBER));
      Target::Ptr trg(new FakeTarget);

      ptime t1(CurrentTime());
      FileTransfer ft(src, trg);
      ft.Exec();
      ptime t2(CurrentTime());

      ASSERT_GT(t2, t1 + millisec(CHUNK_DELAY * CHUNK_NUMBER));
    }

    TEST_F(TestFileTransfer, ExecShouldExitOnTimeout)
    {
      const std::string data(CHUNK_SIZE, '$');
      Source::Ptr src(new FakeSource(data, CHUNK_DELAY, CHUNK_NUMBER));
      Target::Ptr trg(new FakeTarget);

      ptime t1(CurrentTime());
      FileTransfer ft(src, trg);
      ft.Exec(CHUNK_DELAY * CHUNK_NUMBER / 2);
      ptime t2(CurrentTime());

      ASSERT_LT(t2, t1 + millisec(CHUNK_DELAY * CHUNK_NUMBER));
    }

    TEST_F(TestFileTransfer, CancelShouldCancel)
    {
      const std::string data(CHUNK_SIZE, '$');
      Source::Ptr src(new FakeSource(data, CHUNK_DELAY, CHUNK_NUMBER));
      Target::Ptr trg(new FakeTarget);

      ptime t1(CurrentTime());
      FileTransfer ft(src, trg);
      ft.Start();
      usleep(CHUNK_DELAY * CHUNK_NUMBER / 2);
      ft.Cancel();
      ptime t2(CurrentTime());

      ASSERT_LT(t2, t1 + millisec(CHUNK_DELAY * CHUNK_NUMBER));
    }

    TEST_F(TestFileTransfer, SecondWaitShouldContinueWaiting)
    {
      const std::string data(CHUNK_SIZE, '$');
      Source::Ptr src(new FakeSource(data, CHUNK_DELAY, CHUNK_NUMBER));
      FakeTarget::Ptr trg(new FakeTarget);

      FileTransfer ft(src, trg);
      ft.Start();
      ft.Wait(CHUNK_DELAY * CHUNK_NUMBER / 2);
      ft.Wait();

      ASSERT_EQ(CHUNK_SIZE * CHUNK_NUMBER, trg.get()->GetData().size());
    }
  }
}