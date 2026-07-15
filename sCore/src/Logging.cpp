#include "sCore/include/Logging.h"

namespace sCore
{
  Logging::Logging()
    : m_sink(std::make_shared<spdlog::sinks::stderr_color_sink_mt>())
    , m_logger(std::make_shared<spdlog::logger>("console", m_sink))
  {
    m_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%#] %v");
  }

  void Logging::LogInfo(const std::string& message, const char* file, int line) const
  {
    Log(message, LogLevel::Info, file, line);
  }

  void Logging::LogWarning(const std::string& message, const char* file, int line) const
  {
    Log(message, LogLevel::Warn, file, line);
  }

  void Logging::LogCritical(const std::string& message, const char* file, int line) const
  {
    Log(message, LogLevel::Critical, file, line);
  }

  void Logging::LogDebug(const std::string& message, const char* file, int line) const
  {
    Log(message, LogLevel::Debug, file, line);
  }

  void Logging::LogError(const std::string& message, const char* file, int line) const
  {
    Log(message, LogLevel::Error, file, line);
  }

  void Logging::Log(const std::string& message, LogLevel level, const char* file, int line) const
  {
    spdlog::source_loc loc(file, line, "");
    m_logger->log(loc, spdlog::level::level_enum(level), message);
  }

} // namespace WiCore
