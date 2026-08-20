#include "ConsoleService.h"

#include "ReflectionService.h"

using namespace Nyanners::Services;
Nyanners::Instances::Signal<Nyanners::Core::LogLevel, std::string> ConsoleService::onMessage;

namespace Nyanners::Scripting {
	static auto engineDescriptor = ReflectionDescriptorRegistry::instance()->create_registrator([]() {
		ReflectionService::register_enum("LogLevel", {
			                                 {"Info", static_cast<int>(Core::LogLevel::Info)},
			                                 {"Debug", static_cast<int>(Core::LogLevel::Debug)},
			                                 {"Warning", static_cast<int>(Core::LogLevel::Warning)},
			                                 {"Error", static_cast<int>(Core::LogLevel::Error)},
		                                 });
		ReflectionService::create_descriptor("ConsoleService", {"Instance"})
				.add_method<&ConsoleService::log_lua>(
					"log", Null, {
						{
							"message", String
						}
					}
				)
		.add_constructor<ConsoleService>();
	});
}

void ConsoleService::log(const Core::LogLevel level, const std::string &message) {
	onMessage.fire(level, message);
}

void ConsoleService::log_lua(std::string message) {
	Core::Logger::log(message);
}
