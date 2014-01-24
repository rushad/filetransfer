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
      static void* ThreadCancelWaitShouldCancelPop(void*);
      static void* ThreadPushShouldWaitWhenQueueIsOverflowed(void*);
      static void* ThreadCancelWaitShouldCancelPush(void*);
    };

    struct ThreadData
    {
      ThreadData(size_t maxSize = DefaultMaxQueueSize)
        : Q(maxSize)
        , Proof(0)
      {
      }
      Queue Q;
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

    TEST_F(TestQueue, CheckPopOverflow)
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

    void* TestQueue::ThreadCancelWaitShouldCancelPop(void* data)
    {
      ThreadData* td = static_cast<ThreadData*>(data);
      td->Q.Pop(1);
      td->Proof = 123;
      return 0;
    }

    TEST_F(TestQueue, CancelWaitShouldCancelPop)
    {
      ThreadData td;

      pthread_t id;
      pthread_create(&id, 0, ThreadCancelWaitShouldCancelPop, &td);
      usleep(100);

      td.Q.Cancel();
      pthread_join(id, 0);

      ASSERT_EQ(123, td.Proof);
    }

    TEST_F(TestQueue, PushShouldIncreaseSize)
    {
      const std::string str("0123456789");

      Queue q;
      q.Push(MakeChunk(str));

      ASSERT_EQ(str.size(), q.Size());
    }

    TEST_F(TestQueue, PopShouldNotDecreaseSizeWhenNoChunksWasPopped)
    {
      const std::string str("0123456789");

      Queue q;
      q.Push(MakeChunk(str));

      Queue::Chunk buf = q.Pop(str.size() - 1);

      ASSERT_EQ(str.size(), q.Size());
    }

    TEST_F(TestQueue, PopShouldDecreaseSizeWhenChunksWasPopped)
    {
      const std::string str("0123456789");

      Queue q;
      q.Push(MakeChunk(str));

      Queue::Chunk buf = q.Pop(str.size());

      ASSERT_EQ(0, q.Size());
    }

    void* TestQueue::ThreadPushShouldWaitWhenQueueIsOverflowed(void* data)
    {
      Queue* q = static_cast<Queue*>(data);
      usleep(100);
      q->Pop(10);
      return 0;
    }

    TEST_F(TestQueue, PushShouldWaitWhenQueueIsOverflowed)
    {
      const std::string strChunk(10, '$');
      const size_t maxSize = 1000;

      Queue q(maxSize);
      while (q.Size() < maxSize)
        q.Push(MakeChunk(strChunk));

      pthread_t id;
      pthread_create(&id, 0, ThreadPushShouldWaitWhenQueueIsOverflowed, &q);

      ptime t1(CurrentTime());
      q.Push(MakeChunk(strChunk));
      ptime t2(CurrentTime());

      pthread_join(id, 0);

      ASSERT_GT(t2, t1 + millisec(100));
    }

    void* TestQueue::ThreadCancelWaitShouldCancelPush(void* data)
    {
      ThreadData* td = static_cast<ThreadData*>(data);
      td->Q.Push(MakeChunk("123"));
      td->Proof = 321;
      return 0;
    }

    TEST_F(TestQueue, CancelWaitShouldCancelPush)
    {
      const size_t maxSize(1000);
      ThreadData td(maxSize);

      td.Q.Push(MakeChunk(std::string(maxSize, '$')));

      pthread_t id;
      pthread_create(&id, 0, ThreadCancelWaitShouldCancelPush, &td);
      usleep(100);

      td.Q.Cancel();
      pthread_join(id, 0);

      ASSERT_EQ(321, td.Proof);
    }
  }
}
