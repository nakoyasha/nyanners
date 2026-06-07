#include "EngineService.h"
#include "core/Logger.h"

#include <source_location>
#include <format>

#include "ReflectionService.h"
#include "scripting/reflections/ReflectionDescriptorRegistry.h"

#if defined(_WIN32) || defined(_WIN64)
	#define WIN32_MEAN_AND_LEAN;
	#include "windows.h"
#elif defined(__APPLE__) || defined(__MACH__)
#elif defined(__linux__)
#endif

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
Nyanners::Core::EnginePlatform EngineService::platform;

EngineService::EngineService(): Instance("EngineService") {
	engineInfo.version = "0.2.5";
	engineInfo.branch = GIT_BRANCH;
	engineInfo.buildTime = __TIMESTAMP__;
#if defined(_WIN32) || defined(_WIN64)
	platform = Core::EnginePlatform::Windows;
#elif defined(__APPLE__) || defined(__MACH__)
	platform = Core::EnginePlatform::Mac;
#elif defined(__linux__)
	platform = Core::EnginePlatform::Linux;
#else
	platform = Core::EnginePlatform::Unknown;
#endif
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

void EngineService::open_url(const std::string &url) {
#if defined(_WIN32) || defined(_WIN64)
	ShellExecuteA(nullptr, "open", url.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
#elif defined(__APPLE__) || defined(__MACH__)
	throw std::runtime_error("Unimplemented for current platform");
#elif defined(__linux__)
	throw std::runtime_error("Unimplemented for current platform");
#endif
}
