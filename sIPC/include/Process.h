#pragma once

#include "sIPC/include/Windows.h"

namespace sIPC::Process
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
      if (handle)
        CloseHandle(handle);
    }

    bool Valid()
    {
      return handle != 0;
    }
  };

  bool processHasModule(unsigned long pid, const std::string& moduleName)
  {
    if (moduleName.empty())
    {
#ifdef DEBUG
      LOG_WARN("Module to search is empty! returning true ...");
#endif
      return true;
    }

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
    if (hSnap == INVALID_HANDLE_VALUE)
      return false;

    MODULEENTRY32 me{};
    me.dwSize = sizeof(me);
    if (Module32First(hSnap, &me))
      do
      {
        std::string module(me.szModule);
        if (module == moduleName)
          return true;
      } while (Module32Next(hSnap, &me));

    return false;
  }

  ProcessHandle open(const std::string& processName, const std::string& moduleName = "")
  {
    ProcessHandle pHandle;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE)
      throw std::runtime_error(Windows::GetWindowsLastError());

    PROCESSENTRY32 pe{};
    pe.dwSize = sizeof(pe);
    bool found = false;
    if (Process32First(hSnap, &pe))
      do
      {
        if (found)
        {
          LOG_WARN("Multiple process that meets the criteria have been found!");
          LOG_INFO("We will open the first match ...");
          return pHandle;
        }

        std::string exeName(pe.szExeFile);
        if (exeName == processName)
          if (processHasModule(pe.th32ProcessID, moduleName))
          { // open process
            HANDLE hProcess = OpenProcess(PROCESS_DUP_HANDLE, FALSE, pe.th32ProcessID);
            pHandle = { processName + moduleName, hProcess, pe.th32ProcessID };
            found = true;
          }
      } while (Process32Next(hSnap, &pe));

    return pHandle;
  }

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
      options.execPath.c_str(), cmd.data(), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, options.workingDir.c_str(), &si, &pi))
      throw std::runtime_error(Windows::GetWindowsLastError());

    // success
    HANDLE hJob = CreateJobObject(nullptr, nullptr);
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION info{};
    info.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    SetInformationJobObject(hJob, JobObjectExtendedLimitInformation, &info, sizeof(info));
    AssignProcessToJobObject(hJob, pi.hProcess);
    return { options.execPath.filename().string() ,pi.hProcess, pi.dwProcessId };
  }

  // kill a process
  void terminate(
    const ProcessHandle& pHandle, unsigned exitCode = 1, std::chrono::seconds timeout = std::chrono::seconds(1))
  {
    if (!TerminateProcess(pHandle.handle, exitCode))
    {
      LOG_ERROR("process {} failed to terminate.", pHandle.name);
      return;
    }

    auto result = WaitForSingleObject(pHandle.handle, static_cast<unsigned long>(timeout.count()));
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
