#include "sService/include/Service.h"
#include "sService/include/Utils.h"

namespace sService
{
  Service::Service(const std::filesystem::path& dllRunner, const std::filesystem::path& workingDir)
    : m_serviceId(std::format("{}-{}", "Service", sIPC::Process::currentProcessId()))
    , m_dllRunnerPath(dllRunner)
    , m_dllRunnerWorkingDir(workingDir)
    , m_ownedServices()
    , m_openedServices()
    , m_metaData(m_serviceId)
  {
    if (!std::filesystem::exists(dllRunner) || !dllRunner.has_filename())
      throw std::runtime_error("path to dllrunner is not valid");
  }

  Service::~Service()
  {
  }

  const std::string& Service::Id()
  {
    return m_serviceId;
  }

  void Service::Initialize()
  {
    LOG_INFO("Initialize Service");
    // fill the shared meta data
    m_metaData->pid = sIPC::Process::currentProcessId();
  }

  void Service::Run()
  {
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
    sIPC::Process::ProcessHandle pHandle;
    try
    { // first let's try to open the service
      pHandle = sIPC::Process::open(m_dllRunnerPath.filename().string(), modulePath.filename().string());
      serviceOpened = pHandle.Valid();
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
        pHandle = sIPC::Process::launch(options);
        serviceCreated = pHandle.Valid();
      }
      catch (const std::exception& e)
      {
        serviceCreated = false;
        LOG_CRITICAL("{}", e.what());
      }
    }
    // error handling
    if (!serviceCreated && !serviceOpened)
      throw std::runtime_error("could not open not create service with module " + modulePath.filename().string());
    if (serviceCreated && serviceOpened)
      throw std::runtime_error("something unexpected has happened");
    
    // make our process meta data available
    m_metaData.DuplicateToProcess(pHandle.handle);
    // now we poll for the target service meta data
    if (
      PollFor([&pHandle]() {
        auto targetId = std::format("{}-{}", "Service", sIPC::Process::processId(pHandle.handle.value));
        auto handle = sIPC::Windows::ReadSharedMemory<ServiceMetaData>(targetId);
        if (handle.Valid())
        { // cast
          auto meta = static_cast<ServiceMetaData*>(handle.mappedAddress);
          LOG_INFO("Connected to Service with PID {}", meta->pid);
        }
        return handle.Valid();
      }))
    {
      // success
      LOG_INFO("Service {} connected!", modulePath.filename().string());
      // store the service
      serviceCreated ? m_ownedServices.push_back(std::move(pHandle)) : m_openedServices.push_back(std::move(pHandle));
    }
    
  }

} // namespace sService
