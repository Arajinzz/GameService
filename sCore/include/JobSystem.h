#pragma once

#include "sCore/include/WorkerThread.h"
#include "sCore/include/Logging.h"
#include "sCore/include/Job.h"

#include <vector>
#include <functional>
#include <thread>
#include <optional>

namespace sCore
{
  class JobSystem
  {
  public:
    static JobSystem& Get()
    {
      static JobSystem instance;
      return instance;
    }

    const JobHandlePtr EnqeueJob(Priority priority, WorkType work);
    const JobHandlePtr EnqeueJob(std::vector<JobHandlePtr> dependencies, Priority priority, WorkType work);
    void OnMultiJobsCompleted(std::vector<JobHandlePtr> dependencies, WorkType work);

    void Shutdown();
    void WaitForJob(JobHandlePtr handle);
    void WaitForAllJobs();

  private:
    JobSystem();
    ~JobSystem();

    bool WorkQueuesEmpty();
    const JobHandlePtr EnqueueJobHelper(WorkType work, std::vector<JobHandlePtr> dependencies, Priority priority);

    bool m_running;
    std::unordered_map<Priority, RingBufferPtr> m_workQueues;
    std::vector<WorkerThreadPtr> m_workers;

  public:
    // delete copy and assignment
    JobSystem(const JobSystem&) = delete;
    JobSystem& operator=(const JobSystem&) = delete;
  };

} // namespace WiCore
