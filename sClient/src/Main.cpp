#include <windows.h>
#include <iostream>

const char* SHM_NAME = "Local\\MySharedMemory";
const DWORD SHM_SIZE = 1024;

int main()
{
  // Open existing shared memory
  HANDLE hMapFile = OpenFileMappingA(
    FILE_MAP_READ,
    FALSE,
    SHM_NAME);

  if (hMapFile == NULL)
  {
    std::cerr << "OpenFileMapping failed: " << GetLastError() << '\n';
    return 1;
  }

  // Map it
  char* pBuf = (char*)MapViewOfFile(
    hMapFile,
    FILE_MAP_READ,
    0,
    0,
    SHM_SIZE);

  if (pBuf == NULL)
  {
    std::cerr << "MapViewOfFile failed: " << GetLastError() << '\n';
    CloseHandle(hMapFile);
    return 1;
  }

  std::cout << "Received: " << pBuf << '\n';

  HANDLE hEvent = OpenEvent(
    EVENT_MODIFY_STATE,
    FALSE,
    "Global\\MyEvent");

  if (hEvent == NULL)
  {
    std::cout << "OpenEvent failed\n";
    return 1;
  }

  // Signal the server
  SetEvent(hEvent);

  std::cout << "Signal sent.\n";

  CloseHandle(hEvent);

  UnmapViewOfFile(pBuf);
  CloseHandle(hMapFile);
}
