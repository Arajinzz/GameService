#include "sCore/include/JobSystem.h"
#include "sCore/include/Logging.h"

namespace sCore
{
  JobSystem::JobSystem()
    : m_running(true)
    , m_workQueues()
    , m_workers()
  {
    unsigned int threadCount = std::thread::hardware_concurrency() - 1;
    LOG_INFO("JobSystem Thread Count {}", threadCount);
    
    // create queues
    m_workQueues[Priority::LOW] = std::make_shared<RingBuffer<JobHandlePtr>>();
    m_workQueues[Priority::NORMAL] = std::make_shared<RingBuffer<JobHandlePtr>>();
    m_workQueues[Priority::HIGH] = std::make_shared<RingBuffer<JobHandlePtr>>();
    m_workQueues[Priority::CRITICAL] = std::make_shared<RingBuffer<JobHandlePtr>>();

    m_workers.reserve(threadCount);
    for (unsigned i = 0; i < threadCount; ++i)
      m_workers.emplace_back(std::make_unique<WorkerThread>(m_workQueues));

    LOG_INFO("JobSystem is running!");
  }

  JobSystem::~JobSystem()
  {
    if (m_running)
      LOG_CRITICAL("the jobsystem was not terminted correctly, please call Shutdown function");
  }

  const JobHandlePtr JobSystem::EnqueueJobHelper(WorkType work, std::vector<JobHandlePtr> dependencies, Priority priority)
  {
    JobGuard guard;

    // create the job handle
    auto handle = JobHandle::CreateJobHandle(work, dependencies);
    if (dependencies.size() == 0)
      m_workQueues[priority]->push(handle);
    else
    {
      for (auto dependency : dependencies)
      {
        dependency->OnComplete([this, handle, priority]() {
          bool allDependenciesFinished = true;
          for (auto handleDependency : handle->GetDependencies())
            allDependenciesFinished = allDependenciesFinished && handleDependency->Finished();
          handle->GetLock().lock();
          if (allDependenciesFinished && !handle->Ready())
          {
            handle->SetReady();
            handle->GetLock().unlock();
            m_workQueues[priority]->push(handle);
          }
        });
      }
    }

    for (auto& worker : m_workers)
    { // wake up a worker to take the job
      if (worker->IsSleeping())
      {
        worker->Notify();
        return handle;
      }
    }

    return handle;
  }

  bool JobSystem::WorkQueuesEmpty()
  {
    bool empty = true;
    unsigned count = Priority::COUNT;
    for (unsigned i = 0; i < count; ++i)
      empty = empty && m_workQueues[(Priority)i]->empty();
    return empty;
  }

  const JobHandlePtr JobSystem::EnqeueJob(Priority priority, WorkType work)
  {
    // create the job handle
    std::vector<JobHandlePtr> dependencies;
    return EnqueueJobHelper(work, dependencies, priority);
  }

  const JobHandlePtr JobSystem::EnqeueJob(std::vector<JobHandlePtr> dependencies, Priority priority, WorkType work)
  {
    return EnqueueJobHelper(work, dependencies, priority);
  }

  void JobSystem::OnMultiJobsCompleted(std::vector<JobHandlePtr> dependencies, WorkType work)
  {
    // the shared_ptr will not be freed, because it will be owned by the dependencies
    // dependencies will hold a reference to the dependees
    (void)EnqueueJobHelper(work, dependencies, Priority::HIGH);
  }

  void JobSystem::Shutdown()
  {
    WaitForAllJobs();
    for (auto& worker : m_workers)
      worker->Stop();
    LOG_INFO("JobSystem shutdown!");
    m_running = false;
  }

  void JobSystem::WaitForJob(JobHandlePtr handle)
  {
    std::mutex m;
    std::condition_variable cv;

    std::thread thread = std::thread([&m, &cv, handle]() {
      handle->OnComplete([&cv] {
        cv.notify_all();
      });

      std::unique_lock<std::mutex> lock(m);
      cv.wait(lock, [handle] { return handle->Finished(); });
    });
    thread.join();
  }

  void JobSystem::WaitForAllJobs()
  {
    // block
    bool allWorkersSleeping = false;
    while (!allWorkersSleeping)
    {
      allWorkersSleeping = true;
      for (auto& worker : m_workers)
        allWorkersSleeping = allWorkersSleeping && worker->IsSleeping();
    }
  }

} // namespace sCore
