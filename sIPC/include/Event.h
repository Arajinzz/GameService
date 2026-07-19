#pragma once

#include "sipc_export.h"
#include "sCore/include/Logging.h"

#include <windows.h>
#include <string.h>
#include <chrono>
#include <memory>

namespace sIPC
{
  enum class SignalType
  {
    Heartbeat,
    Shutdown
  };

  constexpr const char* ToName(SignalType type)
  {
    switch (type)
    {
    case SignalType::Heartbeat:
      return "Local\\Heartbeat";
    case SignalType::Shutdown:
      return "Local\\Shutdown";
    }
    return "";
  }

  class SignalEmitter
  {
  public:
    static std::shared_ptr<SignalEmitter> CreateSignal(const SignalType& type)
    {
      // Workaround to create shared_ptr out of JobHandle with protected constructor
      struct EnableSharedPtr : public SignalEmitter
      {
      public:
        EnableSharedPtr(const SignalType& type)
          : SignalEmitter(type)
        {
        }
      };
      return std::make_shared<EnableSharedPtr>(type);
    }

    virtual ~SignalEmitter()
    {
      CloseHandle(m_eventHandle);
    }

    void Emit()
    {
      SetEvent(m_eventHandle);
    }

  protected:
    explicit SignalEmitter(const SignalType& sigType)
      : m_signalType(sigType)
    {
      const char* signalName = ToName(m_signalType);
      m_eventHandle = CreateEventA(nullptr, FALSE, FALSE, signalName);
      if (!m_eventHandle)
      {
        // it shouldn't fail
        LOG_WARN("Failed to create Signal {}", signalName);
        throw std::runtime_error("failed to create signal");
      }
    }

  private:
    HANDLE m_eventHandle;
    SignalType m_signalType;
  };

  class SignalReceiver
  {
  public:
    static std::shared_ptr<SignalReceiver> CreateSignal(const SignalType& type)
    {
      // Workaround to create shared_ptr out of JobHandle with protected constructor
      struct EnableSharedPtr : public SignalReceiver
      {
      public:
        EnableSharedPtr(const SignalType& type)
          : SignalReceiver(type)
        {
        }
      };
      return std::make_shared<EnableSharedPtr>(type);
    }

    virtual ~SignalReceiver()
    {
      CloseHandle(m_eventHandle);
    }

    bool Receive(std::chrono::milliseconds timeout)
    {
      // block
      auto start = std::chrono::steady_clock::now();
      while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() <= timeout.count())
      {
        if (!m_eventHandle)
        {
          LOG_WARN("invalid signal cannot be received, trying to reopen signal");
          OpenSignal();
        }
        if (WaitForSingleObject(m_eventHandle, INFINITE) == WAIT_OBJECT_0)
          return true;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
      }
      return false;
    }

    bool Poll()
    {
      if (!m_eventHandle)
      {
        LOG_WARN("invalid signal cannot be polled, trying to reopen signal");
        OpenSignal();
        return false;
      }
      // don't block
      return WaitForSingleObject(m_eventHandle, 0) == WAIT_OBJECT_0;
    }

  protected:
    explicit SignalReceiver(const SignalType& sigType)
      : m_signalType(sigType)
    {
      OpenSignal();
    }
    
    void OpenSignal()
    {
      const char* signalName = ToName(m_signalType);
      m_eventHandle = OpenEventA(SYNCHRONIZE, FALSE, signalName);
      if (!m_eventHandle)
      {
        LOG_WARN("Failed to open Signal {}", signalName);
      }
    }

  private:
    HANDLE m_eventHandle;
    SignalType m_signalType;
  };

} // namespace sIPC
