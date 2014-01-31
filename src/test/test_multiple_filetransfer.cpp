#include "test_common.h"
#include "test_fakesource.h"
#include "test_faketarget.h"

#include "../multiple_filetransfer.h"

#include <gtest/gtest.h>

#include <string>

namespace FileTransfer
{
  namespace Test
  {
    class TestMultipleFileTransfer : public ::testing::Test
    {
    };

    TEST_F(TestMultipleFileTransfer, ShouldTransferData)
    {
      const std::string data(CHUNK_SIZE, '$');
      Source::Vector srcVector;
      srcVector.push_back(Source::Ptr(new FakeSource(data, CHUNK_DELAY, CHUNK_NUMBER)));
      Target::Vector trgVector;
      trgVector.push_back(Target::Ptr(new FakeTarget));

      MultipleFileTransfer ft(srcVector, trgVector);
      ft.Start();
      ft.Wait();

      ASSERT_EQ(CHUNK_SIZE * CHUNK_NUMBER, (static_cast<FakeTarget*>(trgVector[0].get()))->Data.size());
    }

    TEST_F(TestMultipleFileTransfer, ShouldTransferDataMultiple)
    {
      const std::string data(CHUNK_SIZE, '$');

      Source::Vector srcVector;
      srcVector.push_back(Source::Ptr(new FakeSource(data, CHUNK_DELAY, CHUNK_NUMBER)));
      srcVector.push_back(Source::Ptr(new FakeSource(data, CHUNK_DELAY, CHUNK_NUMBER)));

      Target::Vector trgVector;
      trgVector.push_back(Target::Ptr(new FakeTarget));
      trgVector.push_back(Target::Ptr(new FakeTarget));

      MultipleFileTransfer ft(srcVector, trgVector);
      ft.Start();
      ft.Wait();

      ASSERT_EQ(CHUNK_SIZE * CHUNK_NUMBER, (static_cast<FakeTarget*>(trgVector[0].get()))->Data.size());
      ASSERT_EQ(CHUNK_SIZE * CHUNK_NUMBER, (static_cast<FakeTarget*>(trgVector[1].get()))->Data.size());
    }

    TEST_F(TestMultipleFileTransfer, StartShouldNotWait)
    {
      const std::string data(CHUNK_SIZE, '$');

      Source::Vector srcVector;
      srcVector.push_back(Source::Ptr(new FakeSource(data, CHUNK_DELAY, CHUNK_NUMBER)));
      srcVector.push_back(Source::Ptr(new FakeSource(data, CHUNK_DELAY, CHUNK_NUMBER)));

      Target::Vector trgVector;
      trgVector.push_back(Target::Ptr(new FakeTarget));
      trgVector.push_back(Target::Ptr(new FakeTarget));

      ptime t1(CurrentTime());
      MultipleFileTransfer ft(srcVector, trgVector);
      ft.Start();
      ptime t2(CurrentTime());

      ASSERT_LT(t2, t1 + millisec(CHUNK_DELAY * CHUNK_NUMBER));
    }

    TEST_F(TestMultipleFileTransfer, WaitShouldWait)
    {
      const std::string data(CHUNK_SIZE, '$');

      Source::Vector srcVector;
      srcVector.push_back(Source::Ptr(new FakeSource(data, CHUNK_DELAY, CHUNK_NUMBER)));
      srcVector.push_back(Source::Ptr(new FakeSource(data, CHUNK_DELAY, CHUNK_NUMBER)));

      Target::Vector trgVector;
      trgVector.push_back(Target::Ptr(new FakeTarget));
      trgVector.push_back(Target::Ptr(new FakeTarget));

      ptime t1(CurrentTime());
      MultipleFileTransfer ft(srcVector, trgVector);
      ft.Start();
      ft.Wait();
      ptime t2(CurrentTime());

      ASSERT_GT(t2, t1 + millisec(CHUNK_DELAY * CHUNK_NUMBER));
    }

    TEST_F(TestMultipleFileTransfer, WaitShouldExitOnTimeout)
    {
      const std::string data(CHUNK_SIZE, '$');

      Source::Vector srcVector;
      srcVector.push_back(Source::Ptr(new FakeSource(data, CHUNK_DELAY, CHUNK_NUMBER)));
      srcVector.push_back(Source::Ptr(new FakeSource(data, CHUNK_DELAY, CHUNK_NUMBER)));

      Target::Vector trgVector;
      trgVector.push_back(Target::Ptr(new FakeTarget));
      trgVector.push_back(Target::Ptr(new FakeTarget));

      ptime t1(CurrentTime());
      MultipleFileTransfer ft(srcVector, trgVector);
      ft.Start();
      ft.Wait(CHUNK_DELAY * CHUNK_NUMBER / 2);
      ptime t2(CurrentTime());

      ASSERT_LT(t2, t1 + millisec(CHUNK_DELAY * CHUNK_NUMBER));
    }

    TEST_F(TestMultipleFileTransfer, ExecShouldWait)
    {
      const std::string data(CHUNK_SIZE, '$');

      Source::Vector srcVector;
      srcVector.push_back(Source::Ptr(new FakeSource(data, CHUNK_DELAY, CHUNK_NUMBER)));
      srcVector.push_back(Source::Ptr(new FakeSource(data, CHUNK_DELAY, CHUNK_NUMBER)));

      Target::Vector trgVector;
      trgVector.push_back(Target::Ptr(new FakeTarget));
      trgVector.push_back(Target::Ptr(new FakeTarget));

      ptime t1(CurrentTime());
      MultipleFileTransfer ft(srcVector, trgVector);
      ft.Exec();
      ptime t2(CurrentTime());

      ASSERT_GT(t2, t1 + millisec(CHUNK_DELAY * CHUNK_NUMBER));
    }

    TEST_F(TestMultipleFileTransfer, ExecShouldExitOnTimeout)
    {
      const std::string data(CHUNK_SIZE, '$');

      Source::Vector srcVector;
      srcVector.push_back(Source::Ptr(new FakeSource(data, CHUNK_DELAY, CHUNK_NUMBER)));
      srcVector.push_back(Source::Ptr(new FakeSource(data, CHUNK_DELAY, CHUNK_NUMBER)));

      Target::Vector trgVector;
      trgVector.push_back(Target::Ptr(new FakeTarget));
      trgVector.push_back(Target::Ptr(new FakeTarget));

      ptime t1(CurrentTime());
      MultipleFileTransfer ft(srcVector, trgVector);
      ft.Exec(CHUNK_DELAY * CHUNK_NUMBER / 2);
      ptime t2(CurrentTime());

      ASSERT_LT(t2, t1 + millisec(CHUNK_DELAY * CHUNK_NUMBER));
    }

    TEST_F(TestMultipleFileTransfer, CancelShouldCancel)
    {
      const std::string data(CHUNK_SIZE, '$');

      Source::Vector srcVector;
      srcVector.push_back(Source::Ptr(new FakeSource(data, CHUNK_DELAY, CHUNK_NUMBER)));
      srcVector.push_back(Source::Ptr(new FakeSource(data, CHUNK_DELAY, CHUNK_NUMBER)));

      Target::Vector trgVector;
      trgVector.push_back(Target::Ptr(new FakeTarget));
      trgVector.push_back(Target::Ptr(new FakeTarget));

      ptime t1(CurrentTime());
      MultipleFileTransfer ft(srcVector, trgVector);
      ft.Start();
      usleep(CHUNK_DELAY * CHUNK_NUMBER / 2);
      ft.Cancel();
      ptime t2(CurrentTime());

      ASSERT_LT(t2, t1 + millisec(CHUNK_DELAY * CHUNK_NUMBER));
    }

    TEST_F(TestMultipleFileTransfer, SecondWaitShouldContinueWaiting)
    {
      const std::string data(CHUNK_SIZE, '$');

      Source::Vector srcVector;
      srcVector.push_back(Source::Ptr(new FakeSource(data, CHUNK_DELAY, CHUNK_NUMBER)));
      srcVector.push_back(Source::Ptr(new FakeSource(data, CHUNK_DELAY, CHUNK_NUMBER)));

      Target::Vector trgVector;
      trgVector.push_back(Target::Ptr(new FakeTarget));
      trgVector.push_back(Target::Ptr(new FakeTarget));

      MultipleFileTransfer ft(srcVector, trgVector);
      ft.Start();
      ft.Wait(CHUNK_DELAY * CHUNK_NUMBER / 2);
      ft.Wait();

      ASSERT_EQ(CHUNK_SIZE * CHUNK_NUMBER, (static_cast<FakeTarget*>(trgVector[0].get()))->Data.size());
      ASSERT_EQ(CHUNK_SIZE * CHUNK_NUMBER, (static_cast<FakeTarget*>(trgVector[1].get()))->Data.size());
    }
  }
}
