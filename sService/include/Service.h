#pragma once

#include "sservice_export.h"

#include "sIPC/include/Process.h"
#include "sIPC/include/SharedMemory.h"
#include "sIPC/include/Event.h"
#include "sCore/include/SingleInstance.h"

#define MAX_PROCESS 8

// defines meta data of this service
struct ServiceMetaData
{
  unsigned pid = 0;
  unsigned processCount = 0;
  unsigned processes[MAX_PROCESS] = {0};
  void* heartbeat[MAX_PROCESS] = {0};
};

class ServiceMetaLogic
{
public:
  explicit ServiceMetaLogic(ServiceMetaData* data)
    : m_data(data)
  {
  }

  virtual ~ServiceMetaLogic()
  {
  }

  void SetPid(unsigned pid)
  {
    m_data->pid = pid;
  }

  unsigned GetPid()
  {
    return m_data->pid;
  }

  void AddProcess(unsigned newPid, sIPC::WinHandle heartbeat)
  {
    m_data->processes[m_data->processCount] = newPid;
    m_data->heartbeat[m_data->processCount] = heartbeat.get();
    m_data->processCount++;
  }

  void RemoveProcess(unsigned newPid)
  {
    unsigned counter = 0;
    unsigned alignedProcess[MAX_PROCESS] = { 0 };
    void* alignedHeartbeat[MAX_PROCESS] = { 0 };
    unsigned deletedCount = 0;
    for (unsigned i = 0; i < m_data->processCount; ++i)
    {
      if (m_data->processes[i] == newPid)
      {
        m_data->processes[i] = 0;
        deletedCount++;
      }
      if (m_data->processes[i])
      {
        alignedProcess[counter] = m_data->processes[i];
        alignedHeartbeat[counter] = m_data->heartbeat[i];
        counter++;
      }
    }
    memcpy(m_data->processes, alignedProcess, sizeof(alignedProcess));
    memcpy(m_data->heartbeat, alignedHeartbeat, sizeof(alignedHeartbeat));
    m_data->processCount -= deletedCount;
  }

  unsigned GetProcessCount()
  {
    return m_data->processCount;
  }

  std::pair<unsigned, void*> GetProcessInfo(unsigned index)
  {
    return { m_data->processes[index], m_data->heartbeat[index] };
  }

private:
  ServiceMetaData* m_data;

private:
  // delete copy and assignment
  ServiceMetaLogic(const ServiceMetaLogic&) = delete;
  ServiceMetaLogic& operator=(const ServiceMetaLogic&) = delete;
};

namespace sService
{
  // Service class shall define an abstraction to connect to another process
  // doing so it shall establish, a one way communicaton from this process to target process
  class SSERVICE_EXPORT Service : public sCore::SingleInstance<Service>
  {
  public:
    // since we use dlls as services, serviceName shall be the process name
    // in this case this will be the DLLRunner, and module name would be
    // the attached dll to the DLLRunner.
    explicit Service(const std::filesystem::path& dllRunner, const std::filesystem::path& workingDir);
    virtual ~Service();

    // Id of the service
    const std::string& Id();

    // lifetime
    void Initialize();
    void Run();
    void Destroy();

    // connect other services
    void openService(unsigned pid);
    void connectService(const std::filesystem::path& modulePath);

  private:
    bool m_running;
    std::string m_serviceId;
    std::filesystem::path m_dllRunnerPath;
    std::filesystem::path m_dllRunnerWorkingDir;
    std::unordered_map<unsigned, sIPC::Process::ProcessInfo> m_ownedServices;
    std::unordered_map<unsigned, sIPC::Process::ProcessInfo> m_openedServices;
    sIPC::SharedMemory<ServiceMetaData, ServiceMetaLogic> m_metaData;
    sIPC::SharedMemory<sIPC::EventData, sIPC::EventLogic> m_heartbeat;

  private:
    // delete copy and assignment
    Service(const Service&) = delete;
    Service& operator=(const Service&) = delete;
  };

} // namespace sService
