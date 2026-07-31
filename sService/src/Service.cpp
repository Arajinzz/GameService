#include "sService/include/Service.h"
#include "sService/include/Utils.h"

namespace sService
{
  Service::Service(const std::filesystem::path& dllRunner, const std::filesystem::path& workingDir)
    : m_running(true)
    , m_serviceId(std::format("{}-{}", "Service", sIPC::Process::currentProcessId()))
    , m_dllRunnerPath(dllRunner)
    , m_dllRunnerWorkingDir(workingDir)
    , m_ownedServices()
    , m_openedServices()
    , m_metaData(m_serviceId)
    , m_heartbeat()
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
    m_metaData->SetPid(sIPC::Process::currentProcessId());
  }

  void Service::Run()
  {
    // run the main loop
    while (m_running)
    {
      for (unsigned i = 0; i < m_metaData->GetProcessCount(); ++i)
      {
        auto [pid, heartbeat] = m_metaData->GetProcessInfo(i);
        LOG_INFO("Process {} Connected!", pid);
        if (heartbeat)
        {
          LOG_INFO("Heartbeat is valid!");
          // create the logic class
          sIPC::EventData data{ heartbeat };
          sIPC::EventLogic theHeartbeat(&data);
          bool alive = theHeartbeat.Receive(std::chrono::milliseconds(1000));
          if (alive)
            LOG_INFO("process {} is alive", pid);
        }
        // let's connect back
        if (m_ownedServices.contains(pid) || m_openedServices.contains(pid))
          continue;
        openService(pid);
      }
      m_heartbeat->Emit();
      // check for a heartbeat each 1 second
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
  }

  void Service::Destroy()
  {
    LOG_INFO("Destroy Service");
    for (auto& [_, serviceHandle] : m_ownedServices)
      sIPC::Process::terminate(serviceHandle, 0);
  }

  void Service::openService(unsigned pid)
  {
    try
    { // first let's try to open the service
      auto pInfo = sIPC::Process::open(pid);
      // now we poll for the target service meta data
      if (
        PollFor([this, &pInfo, pid]() {
          auto targetId = std::format("{}-{}", "Service", sIPC::Process::processId(pInfo.theHandle.get()));
          auto [handle, memory] = sIPC::Windows::ReadSharedMemory<ServiceMetaData>(targetId);
          auto targetMeta = sIPC::SharedMemory<ServiceMetaData, ServiceMetaLogic>(handle);
          if (targetMeta.Valid())
          { // cast
            LOG_INFO("Connected to Service with PID {}", targetMeta->GetPid());
            // I give my heart to the other process
            targetMeta->AddProcess(sIPC::Process::currentProcessId(), m_heartbeat->DuplicateToProcess(pInfo.theHandle));
          }
          return handle.get();
          }))
      { // store
        m_openedServices[sIPC::Process::processId(pInfo.theHandle.get())] = pInfo;
      }
    }
    catch (const std::exception& e)
    {
      LOG_WARN("{}", e.what());
    }
  }

  void Service::connectService(const std::filesystem::path& modulePath)
  {
    if (!std::filesystem::exists(modulePath) && modulePath.extension().string() != "dll")
      throw std::runtime_error("module path is not valid");

    auto serviceCreated = false;
    auto serviceOpened = false;
    sIPC::Process::ProcessInfo pInfo;
    try
    { // first let's try to open the service
      pInfo = sIPC::Process::open(m_dllRunnerPath.filename().string(), modulePath.filename().string());
      serviceOpened = pInfo.theHandle.get() != 0;
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
        pInfo = sIPC::Process::launch(options);
        serviceCreated = pInfo.theHandle.get() != 0;
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
    // now we poll for the target service meta data
    if (
      PollFor([this, &pInfo]() {
        auto targetId = std::format("{}-{}", "Service", sIPC::Process::processId(pInfo.theHandle.get()));
        auto [handle, memory] = sIPC::Windows::ReadSharedMemory<ServiceMetaData>(targetId);
        auto targetMeta = sIPC::SharedMemory<ServiceMetaData, ServiceMetaLogic>(handle);
        if (targetMeta.Valid())
        { // cast
          LOG_INFO("Connected to Service with PID {}", targetMeta->GetPid());
          // I give my heart to the other process
          targetMeta->AddProcess(sIPC::Process::currentProcessId(), m_heartbeat->DuplicateToProcess(pInfo.theHandle));
        }
        return handle.get();
      }))
    {
      // success
      LOG_INFO("Service {} connected!", modulePath.filename().string());
      // store the service
      auto& where = serviceCreated ? m_ownedServices : m_openedServices;
      // store
      where[sIPC::Process::processId(pInfo.theHandle.get())] = pInfo;
    }
  }

} // namespace sService
