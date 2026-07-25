#pragma once

#include "sipc_export.h"
#include "sCore/include/Logging.h"

#include <windows.h>
#include <string.h>
#include <chrono>
#include <memory>

namespace sIPC
{
  class Mutex
  {
  public:
    explicit Mutex()
    {
      m_handle = CreateMutexA(NULL, FALSE, NULL);
    }

    ~Mutex()
    {
    }

  private:
    HANDLE m_handle;

  private:
    // delete copy and assignment
    Mutex(const Mutex&) = delete;
    Mutex& operator=(const Mutex&) = delete;
  };

  class ReadWriteLock
  {
  public:
    explicit ReadWriteLock()
    {
    }

    ~ReadWriteLock()
    {
    }

  private:
    // delete copy and assignment
    ReadWriteLock(const ReadWriteLock&) = delete;
    ReadWriteLock& operator=(const ReadWriteLock&) = delete;
  };

} // namespace sIPC
