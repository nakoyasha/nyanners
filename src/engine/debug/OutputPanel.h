#pragma once
#include "DebugWindow.h"
#include "instances/services/ConsoleService.h"

struct ConsoleOutput {
	const Nyanners::Core::LogLevel	level;
	const std::string message;
};

namespace Nyanners::Debug::UI {
	class OutputPanel : public Nyanners::Instances::DebugWindow {
	public:
		OutputPanel() : Instance("OutputWindow") {
			Nyanners::Services::ConsoleService::onMessage.connect([this](const Nyanners::Core::LogLevel logLevel, std::string message) {
				messages.push_back({
					.level = logLevel,
					.message = message,
				});
			});
		};
		void draw() override;
	private:
		std::vector<ConsoleOutput> messages;
	};
}