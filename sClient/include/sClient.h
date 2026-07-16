#include "sclient_export.h"

namespace sClient
{
  SCLIENT_EXPORT const char* GetName();
  
#ifdef ENTRY_POINT
  extern "C"
  {
    int SCLIENT_EXPORT LaunchDLL();
  }
#endif

} // namespace sClient
