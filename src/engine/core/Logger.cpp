#include "Logger.h"
#include <print>

namespace Nyanners::Core {
void Logger::log(const std::string_view message, const std::source_location logLocation)
{
  // CLion shows this as an error, I don't know why because it links and runs just fine.
  std::println("[{}:{}] {}", logLocation.file_name(), logLocation.line(), message);
}

}