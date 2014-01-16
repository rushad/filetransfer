#include "../queue.h"

#include <pthread.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>

#include <gtest/gtest.h>

namespace FileTransfer
{
  namespace Test
  {
    using boost::posix_time::ptime;
    using boost::posix_time::millisec;
    class TestQueue : public ::testing::Test
    {
    protected:
      static Queue::Chunk MakeChunk(const std::string& str)
      {
        Queue::Chunk chunk(str.size());
        chunk.assign(str.data(), str.data() + str.size());
        return chunk;
      }
      static Queue::Chunk MakeChunk(const Queue::Chunk& chunk1, const Queue::Chunk& chunk2)
      {
        Queue::Chunk chunk(chunk1);
        chunk.insert(chunk.end(), chunk2.begin(), chunk2.end());
        return chunk;
      }
      static ptime CurrentTime()
      {
        return boost::posix_time::microsec_clock::universal_time();
      }
      static void usleep(unsigned ms)
      {
        boost::this_thread::sleep_for(boost::chrono::milliseconds(ms));
      }
      static void* ThreadPopShouldWaitWhenQueueIsEmpty(void*);
      static void* ThreadPopShouldNotBlockDestructor(void*);

      Queue* Q;
      unsigned Proof;
    };

    TEST_F(TestQueue, QueueShouldPopWhatPushed)
    {
      const std::string tpl("0123456789");

      Queue q;

      q.Push(MakeChunk(tpl));

      Queue::Chunk buf = q.Pop(tpl.size());

      ASSERT_EQ(MakeChunk(tpl), buf);
    }

    TEST_F(TestQueue, Push2Pop1)
    {
      const std::string str1("aaaaa");
      const std::string str2("bbbbb");

      Queue q;

      q.Push(MakeChunk(str1));
      q.Push(MakeChunk(str2));

      Queue::Chunk buf = q.Pop(str1.size() + str2.size());

      ASSERT_EQ(MakeChunk(str1 + str2), buf);
    }

    TEST_F(TestQueue, Push2Pop2Different)
    {
      const std::string str1("aaaaaaaaaa");
      const std::string str2("bbbbbbbbbb");
      const size_t firstPopSize = 7;

      Queue q;

      q.Push(MakeChunk(str1));
      q.Push(MakeChunk(str2));

      Queue::Chunk buf1 = q.Pop(firstPopSize);
      Queue::Chunk buf2 = q.Pop(str1.size() + str2.size() - firstPopSize);

      ASSERT_EQ(MakeChunk(str1 + str2), MakeChunk(buf1, buf2));
    }

    TEST_F(TestQueue, Push2Pop3)
    {
      const std::string str1("aaaaaaaaaa");
      const std::string str2("bbbbbbbbbb");
      const size_t firstPopSize = 7;
      const size_t secondPopSize = 6;

      Queue q;

      q.Push(MakeChunk(str1));
      q.Push(MakeChunk(str2));

      Queue::Chunk buf1 = q.Pop(firstPopSize);
      Queue::Chunk buf2 = q.Pop(secondPopSize);
      Queue::Chunk buf3 = q.Pop(str1.size() + str2.size() - (firstPopSize + secondPopSize));

      ASSERT_EQ(MakeChunk(str1 + str2), MakeChunk(buf1, MakeChunk(buf2, buf3)));
    }

    TEST_F(TestQueue, Push3Pop3Different)
    {
      const std::string str1("aaaaaaaaaa");
      const std::string str2("bbbbbbbbbb");
      const std::string str3("cccccccccc");
      const size_t firstPopSize = 7;
      const size_t secondPopSize = 16;

      Queue q;

      q.Push(MakeChunk(str1));
      q.Push(MakeChunk(str2));
      q.Push(MakeChunk(str3));

      Queue::Chunk buf1 = q.Pop(firstPopSize);
      Queue::Chunk buf2 = q.Pop(secondPopSize);
      Queue::Chunk buf3 = q.Pop(str1.size() + str2.size() - (firstPopSize + secondPopSize));

      ASSERT_EQ(MakeChunk(str1 + str2 + str3), MakeChunk(buf1, MakeChunk(buf2, buf3)));
    }

    TEST_F(TestQueue, PopOverflow)
    {
      const std::string str("aaaaaaaaaa");

      Queue q;

      q.Push(MakeChunk(str));

      Queue::Chunk buf = q.Pop(str.size() + 1);

      ASSERT_EQ(MakeChunk(str), buf);
    }

    void* TestQueue::ThreadPopShouldWaitWhenQueueIsEmpty(void* data)
    {
      Queue* q = static_cast<Queue*>(data);
      usleep(100);
      q->Push(MakeChunk("asdf"));
      return 0;
    }

    TEST_F(TestQueue, PopShouldWaitWhenQueueIsEmpty)
    {
      Queue q;

      pthread_t id;
      pthread_create(&id, 0, ThreadPopShouldWaitWhenQueueIsEmpty, &q);

      ptime t1(CurrentTime());
      q.Pop(4);
      ptime t2(CurrentTime());

      pthread_join(id, 0);

      ASSERT_GE(t2, t1 + millisec(100));
    }

    void* TestQueue::ThreadPopShouldNotBlockDestructor(void* data)
    {
      TestQueue* tq = static_cast<TestQueue*>(data);
      tq->Q->Pop(1);
      std::cout << "qweeee" << std::endl;
      tq->Proof = 123;
      return 0;
    }

    TEST_F(TestQueue, PopShouldNotBlockDesctructor)
    {
      Q = new Queue;
      Proof = 0;
      pthread_t id;
      pthread_create(&id, 0, ThreadPopShouldNotBlockDestructor, this);

      usleep(100);
      Q->Cancel();
      pthread_join(id, 0);
      delete Q;

      ASSERT_EQ(123, Proof);
    }
  }
}
