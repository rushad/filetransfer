#include "test_common.h"

#include "../bytes_observer.h"

#include <gtest/gtest.h>

#include <string>

namespace FileTransfer
{
  namespace Test
  {
    class TestBytesObserver : public ::testing::Test
    {
    };

    TEST_F(TestBytesObserver, ShouldAccumulateBytes)
    {
      FakeProgressCalculator progress;
      BytesObserver observer(progress);

      observer.UpdateProgress(10);
      ASSERT_EQ(10, observer.GetBytes());

      observer.UpdateProgress(10);
      ASSERT_EQ(20, observer.GetBytes());
    }

    TEST_F(TestBytesObserver, ShouldCallCalculator)
    {
      FakeProgressCalculator progress;
      BytesObserver observer(progress);

      observer.UpdateProgress(10);
      ASSERT_EQ(1, progress.Calls);

      observer.UpdateProgress(10);
      ASSERT_EQ(2, progress.Calls);
    }
  }
}