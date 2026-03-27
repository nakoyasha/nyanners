#include "EngineService.h"

#include "core/Logger.h"

#include <source_location>

using namespace Nyanners::Services;

[[noreturn]]
void EngineService::panic(const std::string_view &panicMessage) {
  const auto& location = std::source_location::current();
  Core::Logger::log(std::format("PANIC! From {}\n {}", location.file_name(), panicMessage));
  std::terminate();
}