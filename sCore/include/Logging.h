#pragma once

#include "score_export.h"

// To print filename, line ..etc
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/pattern_formatter.h"

#include <string>
#include <memory>
#include <windows.h>
#include <filesystem>

#define LOG_INFO(msg, ...) \
  sCore::Logging::Instance().LogInfo(std::format(msg, ##__VA_ARGS__), __FILE__, __LINE__)
#define LOG_WARN(msg, ...) \
  sCore::Logging::Instance().LogWarning(std::format(msg, ##__VA_ARGS__), __FILE__, __LINE__)
#define LOG_CRITICAL(msg, ...) \
  sCore::Logging::Instance().LogCritical(std::format(msg, ##__VA_ARGS__), __FILE__, __LINE__)
#define LOG_DEBUG(msg, ...) \
  sCore::Logging::Instance().LogDebug(std::format(msg, ##__VA_ARGS__), __FILE__, __LINE__)
#define LOG_ERROR(msg, ...) \
  sCore::Logging::Instance().LogError(std::format(msg, ##__VA_ARGS__), __FILE__, __LINE__)

namespace sCore
{
  // set this before using Logging class to setup a name
  namespace LogName
  {
    extern SCORE_EXPORT std::string LoggingName;
  }

  enum class LogLevel
  {
    Info = spdlog::level::info,
    Warn = spdlog::level::warn,
    Critical = spdlog::level::critical,
    Debug = spdlog::level::debug,
    Error = spdlog::level::err
  };

  // Handles Logging using the spdlog library
  class SCORE_EXPORT Logging
  {
  public:
    static Logging& Instance()
    {
      static Logging instance;
      return instance;
    }

    ~Logging() = default;

    // Log functions
    void LogInfo(const std::string& message, const char* file, int line) const;
    void LogWarning(const std::string& message, const char* file, int line) const;
    void LogCritical(const std::string& message, const char* file, int line) const;
    void LogDebug(const std::string& message, const char* file, int line) const;
    void LogError(const std::string& message, const char* file, int line) const;

  private:
    Logging();
    void Log(const std::string& message, LogLevel level, const char* file, int line) const;

  private:
    // spdlog
    std::shared_ptr<spdlog::sinks::stderr_color_sink_mt> m_sink;
    std::shared_ptr<spdlog::logger> m_logger;

    // delete copy and assignment
    Logging(const Logging&) = delete;
    Logging& operator=(const Logging&) = delete;
  };

} // namespace sCore
