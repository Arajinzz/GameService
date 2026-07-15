#pragma once

#include "sCore/include/SpinLock.h"
#include "sCore/include/Logging.h"

#include <functional>
#include <mutex>
#include <optional>
#include <semaphore>

namespace sCore
{
  template <typename DataType, unsigned Count = 1024>
  class RingBuffer
  {
  public:
    RingBuffer()
      : m_spinLock()
      , m_waiting(0)
      , m_semaphore(0)
      , m_head(0)
      , m_tail(0)
      , m_availableSpace(Count)
    {
    }

    ~RingBuffer()
    {
    }

    void push(DataType data)
    {
      m_spinLock.lock();

#ifdef _DEBUG
      auto resumed = false;
#endif

      // full
      while (m_availableSpace.load() == 0)
      { // block
        m_waiting.fetch_add(1);

#ifdef _DEBUG
        LOG_WARN("Total waiting jobs to be scheduled is {}", m_waiting.load());
#endif
        m_spinLock.unlock();

        m_semaphore.acquire();

#ifdef _DEBUG
        resumed = true;
#endif
        m_spinLock.lock();
      }

      // update head
      auto head = m_head.load();
      m_elements[head] = data;
      IncHead();
      m_spinLock.unlock();
    }

    std::optional<DataType> pop()
    {
      m_spinLock.lock();

      // empty
      if (m_availableSpace.load() == Count)
      {
        m_spinLock.unlock();
        return std::nullopt;
      }

      // update tail
      auto tail = m_tail.load();
      auto ret = m_elements[tail];
      m_elements[tail] = DataType{};
      IncTail();

      auto shouldRelease = m_waiting.load() > 0;
      if (shouldRelease)
        m_waiting.fetch_sub(1);

      m_spinLock.unlock();

      if (shouldRelease)
        m_semaphore.release();

      return ret;
    }

    bool empty()
    {
      m_spinLock.lock();
      bool empty = m_availableSpace.load() == Count;
      m_spinLock.unlock();
      return empty;
    }

    unsigned size()
    {
      m_spinLock.lock();
      auto size = Count - m_availableSpace.load();
      m_spinLock.unlock();
      return size;
    }

  private:
    std::binary_semaphore m_semaphore;
    std::atomic<unsigned> m_waiting;
    SpinLock m_spinLock;
    std::atomic<unsigned> m_head;
    std::atomic<unsigned> m_tail;
    std::atomic<unsigned> m_availableSpace;
    DataType m_elements[Count];

    void IncHead()
    {
      auto value = m_head.load(std::memory_order_relaxed);
      m_head.store((value + 1) % Count);
      m_availableSpace.fetch_sub(1);
    }

    void IncTail()
    {
      auto value = m_tail.load(std::memory_order_relaxed);
      m_tail.store((value + 1) % Count);
      m_availableSpace.fetch_add(1);
    }

  public:
    // delete copy and assignment
    RingBuffer(const RingBuffer&) = delete;
    RingBuffer& operator=(const RingBuffer&) = delete;

  };

} // namespace WiCore
