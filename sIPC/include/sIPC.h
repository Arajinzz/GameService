#include "sipc_export.h"

namespace sIPC
{
  SIPC_EXPORT const char* GetName();

#ifdef ENTRY_POINT
  extern "C"
  {
    int SIPC_EXPORT LaunchDLL();
  }
#endif

} // namespace sIPC
