#include "example_export.h"

namespace Example
{
  EXAMPLE_EXPORT const char* GetName();

#ifdef ENTRY_POINT
  extern "C"
  {
    int EXAMPLE_EXPORT LaunchDLL();
  }
#endif

} // namespace Example
