#pragma once

#include "sCore/include/Logging.h"

#include <string>
#include <filesystem>
#include <Windows.h>
#include <tlhelp32.h>
#include <chrono>

namespace sIPC::Windows
{
  std::string GetWindowsLastError()
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
    return std::string(message);
  }

  HANDLE CreateWindowsMutex()
  {
    auto handle = CreateMutex(NULL, FALSE, NULL);
    CloseHandle(handle);
    return handle;
  }

} // namespace sCore
