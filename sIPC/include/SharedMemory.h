#include "sipc_export.h"

#include <iostream>

namespace sIPC
{
  // DataType has to be trivially copiable
  template <typename DataType>
  class SharedMemory
  {
  public:
    explicit SharedMemory()
    {
    }

    ~SharedMemory()
    {
    }

  private:
    // delete copy and assignment
    SharedMemory(const SharedMemory&) = delete;
    SharedMemory& operator=(const SharedMemory&) = delete;
  };

} // namespace sIPC