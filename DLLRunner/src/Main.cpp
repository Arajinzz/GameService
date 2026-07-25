#include <iostream>
#include <windows.h>
#include <filesystem>

int main(int argc, char* argv[])
{
  std::cout << "DLLRunner!" << std::endl;

  if (argc < 3)
  {
    std::cout << "No dll was passed as argument" << std::endl;
    return 1;
  }

  HMODULE dll = LoadLibraryA(argv[2]);
  if (!dll)
  {
    std::cout << "Could not load dll (" << argv[2] << ")" << std::endl;
    return 1;
  }

  std::cout << "Trying to launch " << argv[2] << std::endl;
  
  // set console title
  std::filesystem::path dllPath(argv[2]);
  SetConsoleTitleW(dllPath.filename().c_str());

  using LaunchFunc = int (*)();
  LaunchFunc LaunchDLL = reinterpret_cast<LaunchFunc>(
    GetProcAddress(dll, "LaunchDLL"));

  if (!LaunchDLL)
  {
    std::cout << "LaunchDLL was not found!" << std::endl;
    return 1;
  }

  int exitCode = LaunchDLL();

  FreeLibrary(dll);

  return exitCode;
}
