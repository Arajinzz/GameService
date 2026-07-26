#pragma once

#include "sservice_export.h"

#include "sIPC/include/Process.h"
#include "sCore/include/SingleInstance.h"

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

    // lifetime
    void Initialize();
    void Destroy();

    // connect other services
    void connectService(const std::filesystem::path& modulePath);

  private:
    std::filesystem::path m_dllRunnerPath;
    std::filesystem::path m_dllRunnerWorkingDir;
    std::vector<sIPC::Process::ProcessHandle> m_ownedServices;
    std::vector<sIPC::Process::ProcessHandle> m_openedServices;

  private:
    // delete copy and assignment
    Service(const Service&) = delete;
    Service& operator=(const Service&) = delete;
  };

} // namespace sService
