#include "sService/include/Service.h"
#include "sService/include/Utils.h"

namespace sService
{
  Service::Service(const std::filesystem::path& dllRunner, const std::filesystem::path& workingDir)
    : m_running(true)
    , m_serviceId(std::format("{}-{}", "Service", sIPC::Process::currentProcessId()))
    , m_dllRunnerPath(dllRunner)
    , m_dllRunnerWorkingDir(workingDir)
    , m_myServiceMetaData(nullptr)
    , m_connectedServices()
  {
    if (!std::filesystem::exists(dllRunner) || !dllRunner.has_filename())
      throw std::runtime_error("path to dllrunner is not valid");
    auto myHandle = sIPC::Process::ProcessInfo({ "ThisService", sIPC::CreateWinHandle(GetCurrentProcess(), false) });
    m_myServiceMetaData = std::make_shared<ServiceMeta>(myHandle, true, true /* create pipe */);
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
  }

  void Service::Run()
  {
    // run the main loop
    while (m_running)
    {
      m_myServiceMetaData->ConnectPipe(); // see if there is any new client connection

      if (auto pid = m_myServiceMetaData->NewProcessReceived(); pid != 0)
      {
        LOG_INFO("New Process Connection {} Incoming!", pid);
        if (!m_connectedServices.contains(pid))
          openService(pid); // open back the process
      }

      m_myServiceMetaData->DisconnectPipe();

      // check for a heartbeat each 1 second
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
  }

  void Service::Destroy()
  {
    LOG_INFO("Destroy Service");
    for (auto& [_, serviceMeta] : m_connectedServices)
      if (serviceMeta->ServiceOwned())
        sIPC::Process::terminate(serviceMeta->GetHandle(), 0);
  }

  sIPC::Process::ProcessInfo Service::launchService(const std::filesystem::path& modulePath)
  {
    sIPC::Process::LaunchOptions options = {
        .execPath = m_dllRunnerPath,
        .workingDir = m_dllRunnerWorkingDir,
        .args = std::vector<std::wstring>({L"--dll " + modulePath.wstring()})
    };
    try
    {
      auto pInfo = sIPC::Process::launch(options);
      if (pInfo.theHandle.get() != 0)
        return pInfo; // launched successfully
    }
    catch (const std::exception& e)
    {
      LOG_CRITICAL("{}", e.what());
    }
    // unsuccessful launch
    return { "", nullptr };
  }

  sIPC::Process::ProcessInfo Service::openService(const std::filesystem::path& modulePath)
  {
    try
    { // first let's try to open the service
      auto pInfo = sIPC::Process::open(m_dllRunnerPath.filename().string(), modulePath.filename().string());
      if (pInfo.theHandle.get() != 0)
        return pInfo;
    }
    catch (const std::exception& e)
    {
      LOG_WARN("{}", e.what());
    }
    // unsuccessful open
    return { "", nullptr };
  }

  sIPC::Process::ProcessInfo Service::openService(unsigned pid)
  {
    try
    { // first let's try to open the service
      auto pInfo = sIPC::Process::open(pid);
      // now we poll for the target service meta data
      if (pInfo.theHandle)
      {
        establishConnection(pInfo, false);
        return pInfo;
      }
    }
    catch (const std::exception& e)
    {
      LOG_WARN("{}", e.what());
    }
    return { "", nullptr };
  }

  void Service::establishConnection(sIPC::Process::ProcessInfo pInfo, bool owned)
  {
    // now we poll for the target service meta data
    if (
      PollFor([this, &pInfo]() {
        auto targetId = std::format("{}-{}", "Service", sIPC::Process::processId(pInfo.theHandle.get()));
        // verify that other process has indeed created its own pipe
        auto pipe = sIPC::Windows::OpenWindowsPipe(targetId);
        if (pipe.get())
        { // cast
          LOG_INFO("Pipe of process with pid {} has opened", sIPC::Process::processId(pInfo.theHandle.get()));
          CancelIoEx(pipe.get(), nullptr);
        }
        return pipe.get();
      }))
    { // create meta data
      
      // poll again we have to write our own process to the pipe
      // but the pipes need to be connected and this takes time
      if (PollFor([this, &pInfo, owned]() {
        auto serviceMeta = std::make_shared<ServiceMeta>(pInfo, owned, false /* open pipe */);
        // inform the other process about me
        if (serviceMeta->AddNewProcess(sIPC::Process::currentProcessId()) > 0)
        { // store
          m_connectedServices[sIPC::Process::processId(pInfo.theHandle.get())] = serviceMeta;
          return true;
        }
        return false;
        }))
      { // success
        LOG_INFO("Service {} connected!", pInfo.name);
      }
    }
  }

  void Service::connectService(const std::filesystem::path& modulePath)
  {
    if (!std::filesystem::exists(modulePath) && modulePath.extension().string() != "dll")
      throw std::runtime_error("module path is not valid");
    bool owned = false;
    auto pInfo = openService(modulePath);
    if (!pInfo.theHandle)
    { // if can't open launch it
      pInfo = launchService(modulePath);
      owned = true;
    }
    // error handling
    if (!pInfo.theHandle)
      throw std::runtime_error("could not open nor create service with module " + modulePath.filename().string());
    // establish p2p connection
    establishConnection(pInfo, owned);
  }

} // namespace sService
