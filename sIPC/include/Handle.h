#pragma once

#include "sipc_export.h"

#include <Windows.h>
#include <memory>

namespace sIPC
{
  using WinHandle = std::shared_ptr<void>;
  using MemHandle = std::shared_ptr<void>;

  WinHandle CreateWinHandle(HANDLE handle, bool owner = true)
  {
    return std::shared_ptr<void>(handle, [owner](HANDLE h) {
      if (h && owner)
        CloseHandle(h);
    });
  }

  MemHandle CreateMemHandle(void* memory)
  {
    return std::shared_ptr<void>(memory, [](void* mem) {
      if (mem)
        UnmapViewOfFile(mem);
    });
  }

} // namespace sIPC
