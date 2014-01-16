#include "../downloader.h"

//#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>

#include <gtest/gtest.h>

namespace FileTransfer
{
  namespace Test
  {
    class TestDownloader : public ::testing::Test
    {
    };

    class FakeSource: public Source
    {
    public:
      FakeSource(const std::string& data, unsigned delay)
        : Data(data)
        , Delay(delay)
      {
      }

      void Run(Receiver& rv)
      {
        boost::this_thread::sleep_for(boost::chrono::milliseconds(Delay));
        rv.Receive((void*)Data.data(), 1, Data.size());
      }

    private:
      const std::string Data;
      const unsigned Delay;
    };
/*
    TEST_F(TestDownloader, ReadShouldStartThread)
    {
      Queue q;

      const std::string data(100, '@');
      FakeSource src(data, 2);

      Downloader dl(src, q);
      dl.Start();

      ASSERT_EQ(0, q.GetDataSize());

        boost::this_thread::sleep_for(boost::chrono::milliseconds(5));

      ASSERT_EQ(data, q.GetData());
    }
*/
  }
}
