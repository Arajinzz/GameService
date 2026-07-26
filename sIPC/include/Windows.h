#pragma once

#include "sCore/include/Logging.h"

#include <string>
#include <filesystem>
#include <Windows.h>
#include <tlhelp32.h>
#include <chrono>

namespace sIPC::Windows
{
  struct WinHandle
  {
    HANDLE value;

    WinHandle()
      : value(0)
    {
    }

    WinHandle(HANDLE handle)
      : value(handle)
    {
    }

    WinHandle(WinHandle&& other) noexcept
      : value(other.value)
    {
      // when we move let's release ownership
      other.value = 0;
    }

    WinHandle& operator=(WinHandle&& other) noexcept
    {
      value = other.value;
      if (this != &other)
        other.value = 0;
      return *this;
    }

    ~WinHandle()
    {
      if (value)
        CloseHandle(value);
    }
  };

  struct SharedMemoryHandle
  {
    void* mappedAddress;
    WinHandle winHandle;

    SharedMemoryHandle()
      : mappedAddress(nullptr)
      , winHandle(0)
    {
    }

    SharedMemoryHandle(HANDLE handle, void* map)
      : mappedAddress(map)
      , winHandle(handle)
    {
    }

    SharedMemoryHandle(SharedMemoryHandle&& other) noexcept
      : mappedAddress(other.mappedAddress)
      , winHandle(std::move(other.winHandle))
    {
      // when we move let's release ownership
      other.mappedAddress = nullptr;
      // we don't need to release the win handle, because it is already moved
    }

    SharedMemoryHandle& operator=(SharedMemoryHandle&& other) noexcept
    {
      mappedAddress = other.mappedAddress;
      winHandle = std::move(other.winHandle);
      if (this != &other)
        other.mappedAddress = nullptr;
      return *this;
    }

    ~SharedMemoryHandle()
    {
      if (mappedAddress)
        UnmapViewOfFile(mappedAddress);
    }

    bool Valid()
    {
      return mappedAddress != nullptr && winHandle.value != 0;
    }
  };

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
      GetCurrentProcess(), toDuplicate, toProcess, &remoteHandle, FILE_MAP_ALL_ACCESS, FALSE,0))
    {
      LOG_ERROR("could not duplicate handle {}", GetWindowsLastError());
    }
    return WinHandle(remoteHandle);
  }

  template <typename Type>
  SharedMemoryHandle AllocateSharedMemory(const std::string& name)
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
      LOG_CRITICAL("could not create shared memory {}", sIPC::Windows::GetWindowsLastError());
      return SharedMemoryHandle();
    }

    auto mapped = MapViewOfFile(handle, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(Type));
    if (!mapped)
    {
      LOG_CRITICAL("could not map shared memory {}", sIPC::Windows::GetWindowsLastError());
      CloseHandle(handle);
    }

    return SharedMemoryHandle(handle, mapped);
  }

  template <typename Type>
  SharedMemoryHandle ReadSharedMemory(const std::string& name)
  {
    static_assert(std::is_trivially_copyable_v<Type>);
    static_assert(std::is_standard_layout_v<Type>);

    if (name.empty())
      return SharedMemoryHandle();
    // open
    auto handle = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, name.c_str());
    if (handle == 0)
    {
      LOG_CRITICAL("could not open shared memory {}", sIPC::Windows::GetWindowsLastError());
      return SharedMemoryHandle();
    }
    // get data pointer
    auto mapped = MapViewOfFile(handle, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(Type));
    if (!mapped)
    {
      LOG_CRITICAL("could not map shared memory {}", sIPC::Windows::GetWindowsLastError());
      CloseHandle(handle);
      return SharedMemoryHandle();
    }
    // is it the correct type
    if (!static_cast<Type*>(mapped))
    {
      LOG_CRITICAL("could not cast the mapped shared memory to the specified type");
      UnmapViewOfFile(mapped);
      CloseHandle(handle);
      return SharedMemoryHandle();
    }
    return SharedMemoryHandle(handle, mapped);
  }

} // namespace sCore
