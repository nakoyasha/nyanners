#include "EngineService.h"
#include "core/Logger.h"

#include <source_location>
#include <format>

using namespace Nyanners::Services;

[[noreturn]]
void EngineService::panic(const std::string_view &panicMessage) {
  const auto& location = std::source_location::current();
  Core::Logger::log(std::format("PANIC! From {}\n {}", location.file_name(), panicMessage));

	#ifdef NDEBUG
    std::terminate();
	#else
	__builtin_trap();
	#endif
}

Nyanners::Instances::Signal<Nyanners::DataTypes::Vector2> EngineService::onWindowResized;
Nyanners::Instances::Signal<const sf::Event*> EngineService::onInternalEvent;

void EngineService::handle_event(const sf::Event *event) {
  onInternalEvent.fire(event);

  if (const auto *resizedEvent = event->getIf<sf::Event::Resized>()) {
    onWindowResized.fire({resizedEvent->size.x, resizedEvent->size.y});
  }
}
