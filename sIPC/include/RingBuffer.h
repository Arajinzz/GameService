#pragma once

#include "sipc_export.h"
#include "sCore/include/Logging.h"

#include <windows.h>
#include <string.h>
#include <chrono>
#include <memory>
#include <type_traits>

namespace sIPC
{
  // data type must be pod
  template <typename DataType, unsigned Count = 1024>
  class RingBuffer
  {
  public:
    explicit RingBuffer()
      : m_head(0)
      , m_tail(Count - 1)
    {
      static_assert(std::is_trivially_copyable_v<DataType>);
      static_assert(std::is_standard_layout_v<DataType>);
    }

    ~RingBuffer()
    {
    }

    void Write(DataType data)
    {

    }

    DataType Read()
    {
    }

  private:
    DataType m_data[Count];
    std::atomic<unsigned> m_head;
    std::atomic<unsigned> m_tail;
    HANDLE readLock;

  private:
    // delete copy and assignment
    RingBuffer(const RingBuffer&) = delete;
    RingBuffer& operator=(const RingBuffer&) = delete;
  };

} // namespace sIPC
