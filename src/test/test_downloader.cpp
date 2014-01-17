#include "../downloader.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>

#include <gtest/gtest.h>

namespace FileTransfer
{
  namespace Test
  {
    class TestDownloader : public ::testing::Test
    {
    protected:
      static Queue::Chunk MakeChunk(const std::string& str)
      {
        Queue::Chunk chunk(str.size());
        chunk.assign(str.data(), str.data() + str.size());
        return chunk;
      }
      static void usleep(unsigned ms)
      {
        boost::this_thread::sleep_for(boost::chrono::milliseconds(ms));
      }
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

    TEST_F(TestDownloader, DownloaderShouldStartThread)
    {
      const std::string data(100, '$');
      FakeSource src(data, 2);
      Queue q;

      Downloader dl(src, q);

      dl.Start();

      ASSERT_EQ(0, q.Size());
    }

    TEST_F(TestDownloader, DownloaderShouldPushDataToQueue)
    {
      const std::string data(100, '$');
      FakeSource src(data, 2);
      Queue q;

      Downloader dl(src, q);

      dl.Start();

      usleep(10);

      ASSERT_EQ(MakeChunk(data), q.Pop(100));
    }
  }
}
