#pragma once
#include <string>
#include <string_view>
#include <source_location>

namespace Nyanners::Core {
  enum LogLevel {
    Info = 0,
    Warning = 1,
    Error = 2,
  };

class Logger {
  Logger() = default;
  ~Logger() = default;

public:
  static void log(const std::string_view message, const std::source_location = std::source_location::current());
  static void log_warning(const std::string_view message, const std::source_location = std::source_location::current());
  static void log_error(const std::string_view message, const std::source_location = std::source_location::current());
  static void log_no_format(LogLevel level, const std::string_view message);

private:
  static void log_internal(LogLevel level, const std::string &message);
};
}