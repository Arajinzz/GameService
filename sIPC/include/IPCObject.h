#pragma once

#include "sipc_export.h"
#include <windows.h>
#include <string.h>
#include <chrono>
#include <memory>
#include <unordered_map>

#include "sCore/include/Logging.h"
#include "sCore/include/Singleton.h"
#include "sIPC/include/Windows.h"
#include "sIPC/include/Handle.h"

namespace sIPC
{
  class IPCObject
  {
  public:
    explicit IPCObject()
      : m_handle(nullptr)
    {
    }

    virtual ~IPCObject()
    {
    }

    WinHandle DuplicateToProcess(const WinHandle target)
    {
      auto pid = sIPC::Process::processId(target.get());
      if (!pid)
        return nullptr;
      WinHandle duplicate = 
        m_duplicated.contains(pid) ? m_duplicated[pid] : sIPC::Windows::DuplicateWindowsHandle(m_handle.get(), target.get());
      m_duplicated[pid] = duplicate;
      return duplicate;
    }

  protected:
    WinHandle m_handle;
    std::unordered_map<unsigned long, WinHandle> m_duplicated;

  private:
    // delete copy and assignment
    IPCObject(const IPCObject&) = delete;
    IPCObject& operator=(const IPCObject&) = delete;
  };

} // namespace sIPC
