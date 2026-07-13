#include "sCore/include/sCore.h"
#include "spdlog/spdlog.h"

#include <windows.h>
#include <iostream>
#include <cstring>

const char* SHM_NAME = "Local\\MySharedMemory";
const DWORD SHM_SIZE = 1024;

int main()
{
  std::cout << sCore::GetVersion() << std::endl;

  // test spdlog
  spdlog::info("Welcome to spdlog!");
  spdlog::error("Some error message with arg: {}", 1);

  // Create shared memory
  HANDLE hMapFile = CreateFileMappingA(
    INVALID_HANDLE_VALUE,   // Use system paging file
    NULL,
    PAGE_READWRITE,
    0,
    SHM_SIZE,
    SHM_NAME);

  if (hMapFile == NULL)
  {
    std::cerr << "CreateFileMapping failed: " << GetLastError() << '\n';
    return 1;
  }

  // Map it into this process
  char* pBuf = (char*)MapViewOfFile(
    hMapFile,
    FILE_MAP_ALL_ACCESS,
    0,
    0,
    SHM_SIZE);

  if (pBuf == NULL)
  {
    std::cerr << "MapViewOfFile failed: " << GetLastError() << '\n';
    CloseHandle(hMapFile);
    return 1;
  }

  strcpy_s(pBuf, SHM_SIZE, "Hello from server!");

  std::cout << "Message written.\n";

  HANDLE hEvent = CreateEvent(
    NULL,       // default security
    FALSE,      // auto-reset
    FALSE,      // initially non-signaled
    "Global\\MyEvent");

  if (hEvent == NULL)
  {
    std::cout << "CreateEvent failed\n";
    return 1;
  }

  std::cout << "Waiting for client...\n";

  // Wait until another process signals the event
  WaitForSingleObject(hEvent, INFINITE);

  std::cout << "Client sent a signal!\n";

  UnmapViewOfFile(pBuf);
  CloseHandle(hMapFile);
}