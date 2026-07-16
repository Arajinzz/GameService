#include "score_export.h"

namespace sCore
{
  SCORE_EXPORT const char* GetName();

#ifdef ENTRY_POINT
  extern "C"
  {
    int SCORE_EXPORT LaunchDLL();
  }
#endif

} // namespace sCore
