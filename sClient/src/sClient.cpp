#include "sClient/include/sClient.h"
#include "sCore/include/sCore.h"
#include "sCore/include/Logging.h"
#include "sIPC/include/Process.h"
#include "sService/include/Service.h"

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

    std::shared_ptr<sService::Service> service = std::make_shared<sService::Service>(
      std::filesystem::path("DLLRunner/DLLRunner.exe"), std::filesystem::current_path());
    service->Initialize();
    service->connectService(std::filesystem::current_path() / std::filesystem::path("sServer.dll"));
    // run
    service->Run();
    // destroy
    service->Destroy();

    return 0;
  }
#endif

} // namespace sClient

