#include "sServer/include/sServer.h"
#include "sCore/include/Logging.h"
#include "sIPC/include/Event.h"

namespace sServer
{
  const char* GetName()
  {
    return "sServer";
  }

#ifdef ENTRY_POINT
  int LaunchDLL()
  {
    // Setup Logging
    sCore::LogName::LoggingName = GetName();
    LOG_INFO("Hello from {}", GetName());

    // open heartbeat signal
    auto heartbeat = sIPC::SignalEmitter::CreateSignal(sIPC::SignalType::Heartbeat);
    while (1)
    {
      LOG_INFO("Send a heartbeat");
      heartbeat->Emit();
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
  }
#endif

} // namespace sServer
