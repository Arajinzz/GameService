#pragma once

#include "sCore/include/Logging.h"

#include <string>
#include <filesystem>
#include <Windows.h>
#include <chrono>

namespace sCore::Process
{
  struct LaunchOptions
  {
    std::filesystem::path execPath;
    std::filesystem::path workingDir;
    std::vector<std::wstring> args;
  };

  struct ProcessHandle
  {
    std::string name;
    HANDLE handle;
    unsigned long pid;

    ~ProcessHandle()
    {
      CloseHandle(handle);
    }
  };

  // launch a new process
  ProcessHandle launch(const LaunchOptions& options)
  {
    STARTUPINFOW si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // make the command line
    std::wstring cmd = L"";
    for (const auto& arg : options.args)
      cmd = cmd + L" " + arg;

    // Start the child process. 
    if (!CreateProcessW(
      options.execPath.c_str(), cmd.data(), NULL, NULL, FALSE, 0, NULL, options.workingDir.c_str(), &si, &pi))
    {
      DWORD error = GetLastError();
      LPSTR message = nullptr;
      FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&message,
        0,
        nullptr);
      throw std::runtime_error(message);
    }
    // success
    return { options.execPath.filename().string() ,pi.hProcess, pi.dwProcessId};
  }

  // kill a process
  void terminate(
    const ProcessHandle& pHandle, unsigned exitCode = 1, std::chrono::seconds timeout = std::chrono::seconds(1))
  {
    if (!TerminateProcess(pHandle.handle, exitCode))
    {
      DWORD error = GetLastError();
      LPSTR message = nullptr;
      FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&message,
        0,
        nullptr);
      LOG_ERROR("process {} failed to terminate.", pHandle.name);
      return;
    }

    DWORD result = WaitForSingleObject(
      pHandle.handle,
      static_cast<DWORD>(timeout.count()));
    switch (result)
    {
    case WAIT_OBJECT_0:
      // Process has exited.
      LOG_INFO("process with {} has been terminated.", pHandle.name);
      break;

    case WAIT_TIMEOUT:
      LOG_ERROR("timeout: process {} could not terminate.", pHandle.name);
      break;

    case WAIT_FAILED:
      DWORD error = GetLastError();
      LPSTR message = nullptr;
      FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&message,
        0,
        nullptr);
      LOG_ERROR("wait for process {} failed.", pHandle.name);
      break;
    }
  }

  uint32_t exitCode(const ProcessHandle& pHandle)
  {
    DWORD exitCode;
    if (!GetExitCodeProcess(pHandle.handle, &exitCode))
      return EXCEPTION_INVALID_HANDLE;
    return exitCode;
  }

  // is it running?
  bool running(const ProcessHandle& pHandle)
  {
    return exitCode(pHandle) == STILL_ACTIVE;
  }

} // namespace sCore
