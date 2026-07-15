#pragma once

#include "sCore/include/RingBuffer.h"
#include "sCore/include/Logging.h"
#include "sCore/include/Job.h"

#include <thread>
#include <mutex>
#include <optional>
#include <semaphore>

namespace sCore
{
  class WorkerThread;

  using WorkerThreadPtr = std::unique_ptr<WorkerThread>;
  using RingBufferPtr = std::shared_ptr<RingBuffer<JobHandlePtr>>;

  class WorkerThread
  {
    inline static unsigned WorkerThreadId;

  public:
    WorkerThread(std::unordered_map<Priority, RingBufferPtr>& workQueues)
      : m_id(WorkerThreadId)
      , m_workQueues(workQueues)
      , m_running(false)
      , m_mutex()
      , m_thread()
      , m_threadIsActive()
      , m_asleep(false)
    {
      WorkerThreadId++;

      m_running = true;

      m_thread = std::thread([this]() {
        while (m_running)
        {
          {
            std::unique_lock<std::mutex> lock(m_mutex);

            m_threadIsActive.wait(lock, [this] {
#ifdef _DEBUG
              if (m_asleep.load())
                LOG_WARN("WorkerThread {} is going to sleep", m_id);
#endif
              return !m_asleep.load(std::memory_order_relaxed) || !m_running.load(std::memory_order_relaxed);
            });
            if (!m_running.load(std::memory_order_relaxed))
              return;
          }

          RunJob();
        }
      });
    }

    ~WorkerThread()
    {
      Stop();
    }

    void Stop()
    {
      m_running.store(false);
      m_threadIsActive.notify_one();
      if (m_thread.joinable())
        m_thread.join();
    }

    void Notify()
    {
      m_asleep.store(false);
      m_threadIsActive.notify_one();
    }

    bool IsSleeping()
    {
      // additional check of WorkQueuesEmpty() will allow workerthreads to wait for all jobs more reliabely
      return m_asleep.load() || WorkQueuesEmpty();
    }

  protected:
    bool WorkQueuesEmpty()
    {
      bool empty = true;
      unsigned count = Priority::COUNT;
      for (unsigned i = 0; i < count; ++i)
        empty = empty && m_workQueues[(Priority)i]->empty();
      return empty;
    }

    JobHandlePtr PopJob()
    {
      unsigned count = Priority::COUNT;
      for (unsigned i = 0; i < count; ++i)
      {
        // we start with critical
        auto job = m_workQueues[(Priority)i]->pop();
        if (job.has_value())
          return job.value();
      }
      return nullptr;
    }

    void RunJob()
    {
      auto job = PopJob();
      if (job)
      {
        m_asleep.store(false);
#ifdef _DEBUG
        LOG_INFO("Job is executed by WorkerThread {}", m_id);
#endif
        job->ExecuteJob();
        // executes all onComplete callbacks
        job->JobFinished();
      }
      m_asleep.store(WorkQueuesEmpty());
    }

  private:
    unsigned m_id;
    std::atomic<bool> m_running;
    std::thread m_thread;
    std::condition_variable m_threadIsActive;
    std::atomic<bool> m_asleep;
    std::mutex m_mutex;
    std::unordered_map<Priority, RingBufferPtr> m_workQueues;

  public:
    // delete copy and assignment
    WorkerThread(const WorkerThread&) = delete;
    WorkerThread& operator=(const WorkerThread&) = delete;
  };

} // namespace WiCore
