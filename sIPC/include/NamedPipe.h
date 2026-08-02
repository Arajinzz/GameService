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

constexpr unsigned PipeBufferSize = 128;

namespace sIPC
{
  class NamedPipe : public IPCObject
  {
  public:
    explicit NamedPipe(const std::string& name, bool open)
      : IPCObject()
      , m_name(name)
      , m_created(!open)
      , ov({})
    {
      if (open)
        m_handle = sIPC::Windows::OpenWindowsPipe(name);
      else
      {
        ov.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
        m_handle = sIPC::Windows::CreateWindowsPipe(name);
      }
    }

    virtual ~NamedPipe()
    {
    }

    void Connect()
    {
      if (!m_created)
      {
        LOG_ERROR("trying to connect named pipe but you are opening the pipe");
        return;
      }

      ResetEvent(ov.hEvent);
      BOOL result = ConnectNamedPipe(m_handle.get(), &ov);

      if (!result)
      {
        DWORD err = GetLastError();

        if (err == ERROR_IO_PENDING)
        {
          // No client yet. Poll here.
          WaitForSingleObject(ov.hEvent, 1000);
        }
        else if (err == ERROR_PIPE_CONNECTED)
        {
          // Client connected immediately
        }
        else
        {
          //LOG_WARN("Error trying to connect namedPipe: {}", sIPC::Windows::GetWindowsLastError());
        }
        
      }
    }

    void Disconnect()
    {
      if (!m_created)
      {
        LOG_ERROR("trying to connect named pipe but you are opening the pipe");
        return;
      }

      DisconnectNamedPipe(m_handle.get());
    }

    unsigned long Write(const std::string& str)
    {
      if (str.size() > PipeBufferSize - 1)
      {
        LOG_WARN("could not write into pipe because size is bigger than {}", PipeBufferSize - 1);
        return 0;
      }

      DWORD numBytesWritten = 0;
      auto result = WriteFile(m_handle.get(), str.c_str(), (DWORD)str.size() * sizeof(char), &numBytesWritten, NULL);
      (void)result;
      //if (result)
      //  LOG_INFO("Number of bytes sent: {}", numBytesWritten);
      //else
      //  LOG_WARN("Failed to send data {}", sIPC::Windows::GetWindowsLastError());
      return numBytesWritten;
    }

    std::string Read()
    {
      char buffer[PipeBufferSize];
      DWORD numBytesRead = 0;
      BOOL result = ReadFile(
        m_handle.get(), buffer, (PipeBufferSize - 1) * sizeof(char), &numBytesRead, NULL);
      (void)result;
      //if (result)
      //  LOG_INFO("Number of bytes read: {}", numBytesRead);
      //else
      //  LOG_WARN("Failed to read data {}", sIPC::Windows::GetWindowsLastError());
      return std::string(buffer);
    }

  private:
    bool m_created;
    std::string m_name;
    OVERLAPPED ov;
    
  private:
    // delete copy and assignment
    NamedPipe(const NamedPipe&) = delete;
    NamedPipe& operator=(const NamedPipe&) = delete;
  };

} // namespace sIPC
