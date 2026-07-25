#include "sClient/include/sClient.h"
#include "sCore/include/sCore.h"
#include "sCore/include/Logging.h"
#include "sIPC/include/Process.h"
#include "sIPC/include/Event.h"

namespace sClient
{
  const char* GetName()
  {
    return "sClient";
  }

#ifdef ENTRY_POINT
  int LaunchDLL()
  {
    // Setup Logging
    sCore::LogName::LoggingName = GetName();

    LOG_WARN("Hello from {}", GetName());
    LOG_WARN("Hello from {}", sCore::GetName());

    sIPC::Process::LaunchOptions options = {
      .execPath = std::filesystem::path("DLLRunner/DLLRunner.exe"),
      .workingDir = std::filesystem::current_path(),
      .args = std::vector<std::wstring>({L"--dll ../sServer.dll"})
    };

    auto pHandle = sIPC::Process::launch(options);
    if (!pHandle.handle)
    { // process is not opened
      LOG_ERROR("Could not open sServer ... Terminate ...");
      return 1;
    }

    // server is alive, check heartbeat
    auto heartbeat = sIPC::SignalReceiver::CreateSignal(sIPC::SignalType::Heartbeat);
    // wait for 3 secs to receive the first heart beat
    bool serverAlive = heartbeat->Receive(std::chrono::milliseconds(3000));
    while (serverAlive)
    {
      // check for a heartbeat each 1 second
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      // poll for a heartbeat
      serverAlive = heartbeat->Poll();
      if (serverAlive)
      {
        LOG_INFO("Received a heartbeat");
      }
    }

    sIPC::Process::terminate(pHandle, 0);
    return 0;
  }
#endif

} // namespace sClient

