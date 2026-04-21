#include "ConsoleService.h"

using namespace Nyanners::Services;
Nyanners::Instances::Signal<Nyanners::Core::LogLevel, std::string> ConsoleService::onMessage;

void ConsoleService::log(const Core::LogLevel level, const std::string &message) {
	onMessage.fire(level, message);
}