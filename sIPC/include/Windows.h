#pragma once

#include <string>
#include <filesystem>
#include <Windows.h>
#include <tlhelp32.h>
#include <chrono>

#include "sCore/include/Logging.h"
#include "sIPC/include/Handle.h"

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

  WinHandle DuplicateWindowsHandle(HANDLE toDuplicate, const HANDLE& toProcess)
  {
    HANDLE remoteHandle;
    if (!DuplicateHandle(
      GetCurrentProcess(), toDuplicate, toProcess, &remoteHandle, FILE_MAP_ALL_ACCESS, FALSE, DUPLICATE_SAME_ACCESS))
    {
      LOG_ERROR("could not duplicate handle {}", GetWindowsLastError());
    }
    return CreateWinHandle(remoteHandle, false);
  }

  template <typename Type>
  std::pair<WinHandle, MemHandle> AllocateSharedMemory(const std::string& name)
  {
    static_assert(std::is_trivially_copyable_v<Type>);
    static_assert(std::is_standard_layout_v<Type>);

    name.empty() ? LOG_INFO("Creating unnamed shared memory")
      : LOG_INFO("Creating named shared memory with name {}", name);

    // create the shared memory
    auto handle = CreateFileMapping(
      INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(Type), name.empty() ? NULL : name.c_str());
    if (handle == 0)
    {
      LOG_CRITICAL("could not create shared memory: {}", sIPC::Windows::GetWindowsLastError());
      return {nullptr, nullptr};
    }

    auto mapped = MapViewOfFile(handle, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(Type));
    if (!mapped)
    {
      LOG_CRITICAL("could not map shared memory: {}", sIPC::Windows::GetWindowsLastError());
      CloseHandle(handle);
    }

    return { CreateWinHandle(handle), CreateMemHandle(mapped) };
  }

  template <typename Type>
  std::pair<WinHandle, MemHandle> ReadSharedMemory(const std::string& name)
  {
    static_assert(std::is_trivially_copyable_v<Type>);
    static_assert(std::is_standard_layout_v<Type>);

    if (name.empty())
      return { nullptr, nullptr };
    // open
    auto handle = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, name.c_str());
    if (handle == 0)
    {
      LOG_CRITICAL("could not open shared memory: {}", sIPC::Windows::GetWindowsLastError());
      return { nullptr, nullptr };
    }
    // get data pointer
    auto mapped = MapViewOfFile(handle, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(Type));
    if (!mapped)
    {
      LOG_CRITICAL("could not map shared memory: {}", sIPC::Windows::GetWindowsLastError());
      CloseHandle(handle);
      return { nullptr, nullptr };
    }
    // is it the correct type
    if (!static_cast<Type*>(mapped))
    {
      LOG_CRITICAL("could not cast the mapped shared memory to the specified type");
      UnmapViewOfFile(mapped);
      CloseHandle(handle);
      return { nullptr, nullptr };
    }
    return { CreateWinHandle(handle), CreateMemHandle(mapped) };
  }

  template <typename Type>
  std::pair<WinHandle, MemHandle> ReadSharedMemory(WinHandle winHandle)
  {
    static_assert(std::is_trivially_copyable_v<Type>);
    static_assert(std::is_standard_layout_v<Type>);

    // get data pointer
    auto mapped = MapViewOfFile(winHandle.get(), FILE_MAP_ALL_ACCESS, 0, 0, sizeof(Type));
    if (!mapped)
    {
      LOG_CRITICAL("could not map shared memory: {}", sIPC::Windows::GetWindowsLastError());
      return {nullptr, nullptr};
    }
    // is it the correct type
    if (!static_cast<Type*>(mapped))
    {
      LOG_CRITICAL("could not cast the mapped shared memory to the specified type");
      UnmapViewOfFile(mapped);
      return { nullptr, nullptr };
    }
    return { winHandle, CreateMemHandle(mapped) };
  }

  WinHandle CreateWindowsEvent()
  {
    LOG_INFO("Create unnamed event");
    // create the event
    auto handle = CreateEventA(nullptr, FALSE, FALSE, NULL);
    if (handle == 0)
    {
      LOG_CRITICAL("could not create event: {}", sIPC::Windows::GetWindowsLastError());
      return nullptr;
    }
    return CreateWinHandle(handle);
  }

  WinHandle CreateWindowsPipe(const std::string& name)
  {
    std::string pipeName = "\\\\.\\pipe\\" + name;
    // Create a pipe to send data
    HANDLE pipe = CreateNamedPipeA(
      pipeName.c_str(), PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, 4096, 4096, 0, NULL);

    if (pipe == NULL || pipe == INVALID_HANDLE_VALUE)
    {
      LOG_CRITICAL("could not create pipe: {}", sIPC::Windows::GetWindowsLastError());
      return nullptr;
    }

    LOG_INFO("pipe {} has been created", pipeName);
    return CreateWinHandle(pipe);
  }

  WinHandle OpenWindowsPipe(const std::string& name)
  {
    std::string pipeName = "\\\\.\\pipe\\" + name;
    HANDLE pipe = CreateFileA(
      pipeName.c_str(), GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (pipe == NULL || pipe == INVALID_HANDLE_VALUE)
    {
      LOG_CRITICAL("could not open pipe: {}", sIPC::Windows::GetWindowsLastError());
      return nullptr;
    }

    LOG_INFO("pipe {} has been opened", pipeName);
    return CreateWinHandle(pipe);
  }

} // namespace sCore
