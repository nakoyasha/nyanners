#pragma once
#include "instances/basic/Signal.h"

// note: this class is empty mainly because
// it's mostly intended as a router for engine internals
// Core::Logger::Log -> DebugOutputPanel in the editor
// Core::Logger is supposed to call log

namespace Nyanners::Services {
	class ConsoleService {
	public:
		static Instances::Signal<Core::LogLevel, std::string> onMessage;
		static void log(Core::LogLevel level, const std::string &message);
	};
}