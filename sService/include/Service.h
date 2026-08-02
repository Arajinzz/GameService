#pragma once

#include "sservice_export.h"

#include "sIPC/include/Process.h"
#include "sIPC/include/SharedMemory.h"
#include "sIPC/include/Event.h"
#include "sCore/include/SingleInstance.h"
#include "sService/include/ServiceMeta.h"

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
    void connectService(const std::filesystem::path& modulePath);
    
  private:
    sIPC::Process::ProcessInfo launchService(const std::filesystem::path& modulePath);
    sIPC::Process::ProcessInfo openService(const std::filesystem::path& modulePath);
    sIPC::Process::ProcessInfo openService(unsigned pid);
    void establishConnection(sIPC::Process::ProcessInfo pInfo, bool owned);

  private:
    bool m_running;
    std::string m_serviceId;
    std::filesystem::path m_dllRunnerPath;
    std::filesystem::path m_dllRunnerWorkingDir;
    std::shared_ptr<ServiceMeta> m_myServiceMetaData;
    std::unordered_map<unsigned, std::shared_ptr<ServiceMeta>> m_connectedServices;

  private:
    // delete copy and assignment
    Service(const Service&) = delete;
    Service& operator=(const Service&) = delete;
  };

} // namespace sService
