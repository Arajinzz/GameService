#include "sIPC/include/sIPC.h"
#include "sCore/include/Logging.h"

namespace sIPC
{
  const char* GetName()
  {
    return "sIPC";
  }

#ifdef ENTRY_POINT
  int LaunchDLL()
  {
    LOG_WARN("Hello from {}", GetName());
    return 0;
  }
#endif

} // namespace sIPC
