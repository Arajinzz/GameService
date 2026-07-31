#pragma once

#include "sIPC/include/Windows.h"
#include "sIPC/include/Handle.h"

namespace sIPC::Process
{
  struct LaunchOptions
  {
    std::filesystem::path execPath;
    std::filesystem::path workingDir;
    std::vector<std::wstring> args;
  };

  struct ProcessInfo
  {
    std::string name;
    WinHandle theHandle;
  };


  std::string GetProcessName(HANDLE hProcess)
  {
    char path[MAX_PATH];
    DWORD size = MAX_PATH;

    if (QueryFullProcessImageNameA(hProcess, 0, path, &size))
    {
      return std::string(path);
    }

    return "";
  }

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

  ProcessInfo open(unsigned pid)
  {
    auto pHandle = OpenProcess(PROCESS_DUP_HANDLE | PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (pHandle == nullptr)
    {
      throw std::runtime_error(std::format("could not open process with pid {}", pid));
    }
    
    return { GetProcessName(pHandle), CreateWinHandle(pHandle) };
  }

  ProcessInfo open(const std::string& processName, const std::string& moduleName = "")
  {
    HANDLE pHandle = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE)
      throw std::runtime_error(Windows::GetWindowsLastError());

    PROCESSENTRY32 pe{};
    pe.dwSize = sizeof(pe);
    bool found = false;
    if (Process32First(hSnap, &pe))
      do
      {
        // skip meb
        if (GetCurrentProcessId() == pe.th32ProcessID)
          continue;

        if (found)
        {
          LOG_WARN("Multiple process that meets the criteria have been found!");
          LOG_INFO("We will open the first match ...");
          return { GetProcessName(pHandle), CreateWinHandle(pHandle) };
        }

        std::string exeName(pe.szExeFile);
        if (exeName == processName)
          if (processHasModule(pe.th32ProcessID, moduleName))
          { // open process
            pHandle = OpenProcess(PROCESS_DUP_HANDLE | PROCESS_QUERY_INFORMATION, FALSE, pe.th32ProcessID);
            found = true;
          }
      } while (Process32Next(hSnap, &pe));

    return { GetProcessName(pHandle), CreateWinHandle(pHandle) };
  }

  // launch a new process
  ProcessInfo launch(const LaunchOptions& options)
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


    return { GetProcessName(pi.hProcess), CreateWinHandle(pi.hProcess) };
  }

  // kill a process
  void terminate(
    const ProcessInfo pInfo, unsigned exitCode = 1, std::chrono::milliseconds timeout = std::chrono::milliseconds(1))
  {
    if (!TerminateProcess(pInfo.theHandle.get(), exitCode))
    {
      LOG_ERROR("process {} failed to terminate.", pInfo.name);
      return;
    }

    auto result = WaitForSingleObject(pInfo.theHandle.get(), static_cast<unsigned long>(timeout.count()));
    switch (result)
    {
    case WAIT_OBJECT_0:
      // Process has exited.
      LOG_INFO("process with {} has been terminated.", pInfo.name);
      break;
    case WAIT_TIMEOUT:
      LOG_ERROR("timeout: process {} could not terminate.", pInfo.name);
      break;
    case WAIT_FAILED:
      LOG_ERROR("wait for process {} failed.", pInfo.name);
      break;
    }
  }

  uint32_t exitCode(const WinHandle pHandle)
  {
    DWORD exitCode;
    if (!GetExitCodeProcess(pHandle.get(), &exitCode))
      return EXCEPTION_INVALID_HANDLE;
    return exitCode;
  }

  // is it running?
  bool running(const WinHandle pHandle)
  {
    return exitCode(pHandle) == STILL_ACTIVE;
  }

  unsigned long processId(HANDLE pHandle)
  {
    auto pid = GetProcessId(pHandle);
    if (!pid)
      LOG_WARN("pid from handle is 0 {}", sIPC::Windows::GetWindowsLastError());
    return pid;
  }

  unsigned long currentProcessId()
  {
    return processId(GetCurrentProcess());
  }

} // namespace sCore
