#pragma once

#include <vector>
#include <functional>
#include <atomic>
#include <memory>
#include <stdexcept>
#include <mutex>

namespace sCore
{
  class JobSystem;
  class JobHandle;
  using WorkType = std::function<void()>;
  using JobHandlePtr = std::shared_ptr<JobHandle>;

  enum Priority
  {
    CRITICAL = 0,
    HIGH = 1,
    NORMAL = 2,
    LOW = 3,
    COUNT = 4,
  };

  struct JobGuard
  {
    JobGuard()
    {
      if (m_isInsideJob)
        throw std::runtime_error("nested job are not allowed!");
      m_isInsideJob = true;
    }

    ~JobGuard()
    {
      m_isInsideJob = false;
    }

  private:
    inline static thread_local bool m_isInsideJob;
  };

  class JobHandle : public std::enable_shared_from_this<JobHandle>
  {
    friend class JobHandle;
    friend class JobSystem;

  private:
    static JobHandlePtr CreateJobHandle(WorkType job, std::vector<JobHandlePtr> dependencies)
    {
      // Workaround to create shared_ptr out of JobHandle with protected constructor
      struct EnableSharedPtr : public JobHandle 
      {
      public:
        EnableSharedPtr(WorkType job, std::vector<JobHandlePtr> dependencies)
          : JobHandle(job, dependencies)
        {
        }
      };

      auto handle = std::make_shared<EnableSharedPtr>(job, dependencies);
      handle->Initialize();
      return handle;
    }

  protected:
    JobHandle(WorkType job, std::vector<JobHandlePtr> dependencies)
      : m_job(job)
      , m_jobReady(false)
      , m_jobFinished(false)
      , m_dependees()
      , m_dependencies(dependencies)
      , m_onCompletion()
      , m_mutex()
    {
      m_jobReady.store(m_dependencies.size() == 0);
    }

    void Initialize()
    {
      for (auto dependency : m_dependencies)
        dependency->AddDependee(shared_from_this());
    }

  public:
    JobHandle()
      : m_job([](){})
      , m_jobFinished(true)
      , m_dependees()
      , m_dependencies()
    {
    }

    ~JobHandle()
    {
    }

    // copy
    JobHandle(const JobHandle& that) = delete;
    JobHandle& operator=(const JobHandle& that) = delete;

    void ExecuteJob()
    {
      // another instance of this is created when we queue jobs
      JobGuard guard; // to guard against nested jobs
      if (m_job)
        m_job();

      m_mutex.lock();
      m_jobFinished.store(true);
      m_mutex.unlock();
    }

    void JobFinished()
    {
      for (auto& callback : m_onCompletion)
        callback();
    }

    void OnComplete(std::function<void()> callback)
    {
      m_mutex.lock();
      if (m_jobFinished.load())
      {
        callback();
        m_mutex.unlock();
        return;
      }
      m_mutex.unlock();

      m_onCompletion.push_back(callback);
    }

    const std::vector<JobHandlePtr>& GetDependees()
    {
      return m_dependees;
    }

    const std::vector<JobHandlePtr>& GetDependencies()
    {
      return m_dependencies;
    }

    void SetReady()
    {
      m_jobReady.store(true);
    }

    bool Finished()
    {
      return m_jobFinished.load();
    }

    bool Ready()
    {
      return m_jobReady.load();
    }

    std::mutex& GetLock()
    {
      return m_mutex;
    }

  protected:
    void AddDependee(JobHandlePtr dependee)
    {
      // check circular dependency
      for (auto dependency : m_dependencies)
        if (dependee == dependency)
          throw std::runtime_error("error! circular dependency!");
      m_dependees.emplace_back(dependee);
    }

  private:
    WorkType m_job;
    std::atomic<bool> m_jobReady;
    std::atomic<bool> m_jobFinished;
    std::vector<JobHandlePtr> m_dependees; // depended by
    std::vector<JobHandlePtr> m_dependencies; // depends on
    std::vector<std::function<void()>> m_onCompletion;
    std::mutex m_mutex;
  };

} // namespace WiCore
