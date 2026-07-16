#include "sServer/include/sServer.h"
#include "sCore/include/Logging.h"

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

    LOG_WARN("Hello from {}", GetName());
    return 0;
  }
#endif

} // namespace sServer
