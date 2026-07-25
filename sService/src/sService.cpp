#include "sService/include/sService.h"
#include "sCore/include/Logging.h"

namespace sService
{
  const char* GetName()
  {
    return "sService";
  }

#ifdef ENTRY_POINT
  int LaunchDLL()
  {
    LOG_WARN("Hello from {}", GetName());
    return 0;
  }
#endif

} // namespace sService
