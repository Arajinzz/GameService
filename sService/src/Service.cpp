#include "sService/include/Service.h"

namespace sService
{
  Service::Service(const std::filesystem::path& dllRunner, const std::filesystem::path& workingDir)
    : m_dllRunnerPath(dllRunner)
    , m_dllRunnerWorkingDir(workingDir)
    , m_ownedServices()
    , m_openedServices()
  {
    if (!std::filesystem::exists(dllRunner) || !dllRunner.has_filename())
      throw std::runtime_error("path to dllrunner is not valid");
  }

  Service::~Service()
  {
  }

  void Service::Initialize()
  {
    LOG_INFO("Initialize Service");
  }

  void Service::Destroy()
  {
    LOG_INFO("Destroy Service");
    for (auto& serviceHandle : m_ownedServices)
      sIPC::Process::terminate(serviceHandle, 0);
  }

  void Service::connectService(const std::filesystem::path& modulePath)
  {
    if (!std::filesystem::exists(modulePath) && modulePath.extension().string() != "dll")
      throw std::runtime_error("module path is not valid");

    auto serviceCreated = false;
    auto serviceOpened = false;
    try
    { // first let's try to open the service
      auto handle = sIPC::Process::open(m_dllRunnerPath.filename().string(), modulePath.filename().string());
      if (handle.Valid())
      {
        serviceOpened = true;
        m_openedServices.push_back(std::move(handle));
      }
    }
    catch (const std::exception& e)
    {
      serviceOpened = false;
      LOG_WARN("{}", e.what());
      LOG_INFO("Trying to create the service ...");
    }

    if (!serviceOpened)
    { // we couldn't open the service so let's create it
      sIPC::Process::LaunchOptions options = {
        .execPath = m_dllRunnerPath,
        .workingDir = m_dllRunnerWorkingDir,
        .args = std::vector<std::wstring>({L"--dll " + modulePath.wstring()})
      };
      try
      {
        auto handle = sIPC::Process::launch(options);
        if (handle.Valid())
        {
          serviceCreated = true;
          m_ownedServices.push_back(std::move(handle));
        }
      }
      catch (const std::exception& e)
      {
        serviceCreated = false;
        LOG_CRITICAL("{}", e.what());
      }
    }
    if (!serviceCreated && !serviceOpened)
      throw std::runtime_error("could not open not create service with module " + modulePath.filename().string());
    LOG_INFO("Service {} connected!", modulePath.filename().string());
  }

} // namespace sService
