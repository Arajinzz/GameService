#include "sServer/include/sServer.h"
#include "sCore/include/Logging.h"
#include "sService/include/Service.h"

namespace sServer
{
  const char* GetName()
  {
    return "sServer";
  }

#ifdef ENTRY_POINT
  int LaunchDLL()
  {
    // Setup Logging
    sCore::LogName::LoggingName = GetName();
    LOG_INFO("Hello from {}", GetName());

    std::shared_ptr<sService::Service> service = std::make_shared<sService::Service>(
      std::filesystem::path("DLLRunner/DLLRunner.exe"), std::filesystem::current_path());
    service->Initialize();
    // run
    service->Run();
    // destroy
    service->Destroy();

    return 0;
  }
#endif

} // namespace sServer
