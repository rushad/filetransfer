#include "test_common.h"
#include "test_fakesource.h"
#include "test_faketarget.h"

#include "../filetransfer.h"

#include <gtest/gtest.h>

namespace FileTransfer
{
  namespace Test
  {
    class TestFileTransfer : public ::testing::Test
    {
    };

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