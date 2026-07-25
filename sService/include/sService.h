#include "sservice_export.h"

namespace sService
{
  SSERVICE_EXPORT const char* GetName();

#ifdef ENTRY_POINT
  extern "C"
  {
    int SSERVICE_EXPORT LaunchDLL();
  }
#endif

} // namespace sService
