#include "sCore/include/sCore.h"
#include "sCore/include/Logging.h"

namespace sCore
{
  const char* GetName()
  {
    return "sCore";
  }

#ifdef ENTRY_POINT
  int LaunchDLL()
  {
    LOG_WARN("Hello from {}", GetName());
    return 0;
  }
#endif

} // namespace sCore
