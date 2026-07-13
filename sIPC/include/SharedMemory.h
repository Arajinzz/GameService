#include "sipc_export.h"

#include <iostream>

namespace sIPC
{
  template <typename MemoryType>
  class SIPC_EXPORT SharedMemory
  {
  public:
    static void Test()
    {
      std::cout << "New Project!" << std::endl;
    }
  };
}