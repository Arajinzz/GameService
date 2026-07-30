#pragma once

#include "sipc_export.h"
#include <windows.h>
#include <string.h>
#include <chrono>
#include <memory>

#include "sCore/include/Logging.h"
#include "sIPC/include/Windows.h"
#include "sIPC/include/IPCObject.h"
#include "sIPC/include/Handle.h"

namespace sIPC
{
  struct EventData
  {
    HANDLE handle = 0;
  };

  class EventLogic : public IPCObject
  {
  public:
    explicit EventLogic(EventData* data)
      : IPCObject()
      , m_data(data)
      , m_owner(false)
    {
      // already exists
      m_owner = m_data->handle == 0;
      if (!m_data->handle)
        m_handle = sIPC::Windows::CreateWindowsEvent();
      else
      {
        m_handle = CreateWinHandle(m_data->handle, false);
      }
      m_data->handle = m_handle.get();
    }

    virtual ~EventLogic()
    {
    }

    void Emit()
    {
      if (!m_owner)
        LOG_WARN("Trying to emit an event but you are not the owner");
      // only the owner can emit
      if (m_owner)
        SetEvent(m_handle.get());
    }

    bool Receive(std::chrono::milliseconds timeout)
    {
      // block
      if (!m_handle.get())
        LOG_WARN("invalid signal cannot be received, trying to reopen signal");
      auto result = WaitForSingleObject(m_handle.get(), (DWORD)timeout.count());
      if (result == WAIT_OBJECT_0)
        return true;
      switch (result)
      {
      case WAIT_TIMEOUT:
        LOG_ERROR("timeout.");
        break;
      case WAIT_FAILED:
        LOG_ERROR("failed {}.", sIPC::Windows::GetWindowsLastError());
        break;
      }
      return false;
    }

    bool Poll()
    {
      if (!m_handle.get())
      {
        LOG_WARN("invalid signal cannot be polled, trying to reopen signal");
        return false;
      }
      // don't block
      return WaitForSingleObject(m_handle.get(), 0) == WAIT_OBJECT_0;
    }

  private:
    bool m_owner;
    EventData* m_data;

  private:
    // delete copy and assignment
    EventLogic(const EventLogic&) = delete;
    EventLogic& operator=(const EventLogic&) = delete;
  };

} // namespace sIPC
