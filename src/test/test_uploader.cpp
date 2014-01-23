#include "../uploader.h"

#include <gtest/gtest.h>

namespace FileTransfer
{
  namespace Test
  {
    class TestUploader : public ::testing::Test
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

    class FakeTarget : public Target
    {
    public:
      FakeTarget()
      {
      }

      virtual bool Run(Transmitter& tr)
      {
        char buffer[100];
        size_t size;
        while (!tr.Cancelled() && (size = tr.Transmit(buffer, sizeof(buffer), 1)))
          Data += std::string(buffer, size);
        return !tr.Cancelled();
      }

      std::string GetData() const
      {
        return Data;
      }

    private:
      std::string Data;
    };

    TEST_F(TestUploader, UploaderShouldStartThread)
    {
      const std::string data(100, '$');
      FakeTarget trg;

      Queue q;
      q.Push(MakeChunk(data));

      Uploader ul(trg, q, data.size());
      ul.Start();

      ASSERT_LT(trg.GetData().size(), data.size());
    }

    TEST_F(TestUploader, UploaderShouldPopDataFromQueue)
    {
      const std::string data(100, '$');
      FakeTarget trg;

      Queue q;
      q.Push(MakeChunk(data));

      Uploader ul(trg, q, data.size());
      ul.Start();
      usleep(2);

      ASSERT_EQ(data, trg.GetData());
    }

    TEST_F(TestUploader, UploaderShouldStopWhenDone)
    {
      const std::string data(100, '$');
      FakeTarget trg;

      Queue q;

      Uploader ul(trg, q, data.size());
      ul.Start();

      q.Push(MakeChunk(data));
      usleep(2);

      q.Push(MakeChunk(data));
      usleep(2);

      ASSERT_EQ(data, trg.GetData());
    }

    TEST_F(TestUploader, CancelShouldStopUploader)
    {
      const std::string data(100, '$');
      FakeTarget trg;

      Queue q;

      Uploader ul(trg, q, data.size() * 2);
      ul.Start();

      q.Push(MakeChunk(data));
      usleep(2);

      ul.Cancel();

      q.Push(MakeChunk(data));
      usleep(2);

      ASSERT_EQ(data, trg.GetData());
    }
  }
}
