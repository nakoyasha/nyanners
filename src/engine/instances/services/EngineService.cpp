#include "EngineService.h"
#include "core/Logger.h"

#include <source_location>
#include <format>

#include "ReflectionService.h"
#include "scripting/reflections/ReflectionDescriptorRegistry.h"

using namespace Nyanners::Services;

namespace Nyanners::Scripting {
	static auto engineDescriptor = Reflection::ReflectionDescriptorRegistry::instance()->create_registrator([]() {
		ReflectionService::create_descriptor("EngineService", {"Instance"})
		.add_property_chained<EngineService, std::string, &EngineService::get_version>("EngineVersion", String)
		.add_property_chained<EngineService, std::string, &EngineService::get_branch>("Branch", String)
		.add_property_chained<EngineService, std::string, &EngineService::get_build_time>("BuildTime", String);
	});
}

Nyanners::Core::EngineInfo EngineService::engineInfo;

EngineService::EngineService(): Instance("EngineService") {
	engineInfo.version = "0.2.5";
	engineInfo.branch = GIT_BRANCH;
	engineInfo.buildTime = __TIMESTAMP__;
}

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
