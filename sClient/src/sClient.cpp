#include "sClient/include/sClient.h"
#include "sCore/include/sCore.h"
#include "sCore/include/Logging.h"
#include "sCore/include/Process.h"

namespace sClient
{
  const char* GetName()
  {
    return "sClient";
  }

#ifdef ENTRY_POINT
  int LaunchDLL()
  {
    // Setup Logging
    sCore::LogName::LoggingName = GetName();

    LOG_WARN("Hello from {}", GetName());
    LOG_WARN("Hello from {}", sCore::GetName());

    sCore::Process::LaunchOptions options = {
      .execPath = std::filesystem::path("DLLRunner/DLLRunner.exe"),
      .workingDir = std::filesystem::current_path(),
      .args = std::vector<std::wstring>({L"--dll ../sServer.dll"})
    };

    auto pHandle = sCore::Process::launch(options);

    auto debug = 0;
    (void)debug;

    return 0;
  }
#endif

} // namespace sClient

