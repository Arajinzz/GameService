#include "sClient/include/sClient.h"
#include "sCore/include/sCore.h"
#include "sCore/include/Logging.h"
#include "sIPC/include/Process.h"
#include "sIPC/include/Event.h"
#include "sService/include/Service.h"

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

    std::shared_ptr<sService::Service> service = std::make_shared<sService::Service>(
      std::filesystem::path("DLLRunner/DLLRunner.exe"), std::filesystem::current_path());
    service->Initialize();
    service->connectService(std::filesystem::current_path() / std::filesystem::path("sServer.dll"));

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

    // destroy
    service->Destroy();

    return 0;
  }
#endif

} // namespace sClient

