#pragma once

#include "sservice_export.h"
#include <ranges>
#include <charconv>

#include "sIPC/include/Process.h"
#include "sIPC/include/SharedMemory.h"
#include "sIPC/include/Event.h"
#include "sIPC/include/NamedPipe.h"

class ServiceMeta
{
public:
  explicit ServiceMeta(sIPC::Process::ProcessInfo processHandle, bool owner, bool createPipe)
    : m_info(processHandle)
    , m_pid(sIPC::Process::processId(processHandle.theHandle.get()))
    , m_id(std::format("{}-{}", "Service", m_pid))
    , m_owned(owner)
    , m_pipe(nullptr)
  {
    // create a namepipe
    m_pipe = std::make_shared<sIPC::NamedPipe>(m_id, !createPipe);
  }

  virtual ~ServiceMeta()
  {
  }

  void ConnectPipe()
  {
    m_pipe->Connect();
  }

  void DisconnectPipe()
  {
    m_pipe->Disconnect();
  }

  bool ServiceOwned()
  {
    return m_owned;
  }

  unsigned long AddNewProcess(unsigned pid)
  {
    std::string command = std::format("new_connection:{}", pid);
    return m_pipe->Write(command);
  }

  unsigned NewProcessReceived()
  {
    auto command = m_pipe->Read();
    if (!command.empty())
    {
      std::string_view input = command;
      auto pos = input.find(':');
      if (pos != std::string_view::npos)
      {
        std::string_view cmdValue = input.substr(0, pos);
        std::string_view value = input.substr(pos + 1);
        if (cmdValue == "new_connection")
          return std::stoul(std::string(value));
      }
    }
    return 0;
  }

  unsigned GetPid()
  {
    return m_pid;
  }

  sIPC::Process::ProcessInfo GetHandle()
  {
    return m_info;
  }

private:
  sIPC::Process::ProcessInfo m_info;
  unsigned m_pid;
  std::string m_id;
  bool m_owned;
  std::shared_ptr<sIPC::NamedPipe> m_pipe;

private:
  // delete copy and assignment
  ServiceMeta(const ServiceMeta&) = delete;
  ServiceMeta& operator=(const ServiceMeta&) = delete;
};

