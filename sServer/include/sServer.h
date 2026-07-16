#include "sserver_export.h"

namespace sServer
{
  SSERVER_EXPORT const char* GetName();

#ifdef ENTRY_POINT
  extern "C"
  {
    int SSERVER_EXPORT LaunchDLL();
  }
#endif

} // namespace sServer
