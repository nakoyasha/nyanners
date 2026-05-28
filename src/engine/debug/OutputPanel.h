#pragma once
#include "DebugWindow.h"
#include "imgui.h"
#include "instances/services/ConsoleService.h"

struct ConsoleOutput {
	const Nyanners::Core::LogLevel	level;
	const std::string message;
};

namespace Nyanners::Debug::UI {
	class OutputPanel : public Nyanners::Instances::DebugWindow {
	public:
		OutputPanel();;
		void draw() override;
	private:
		ImFont *font;
		std::vector<ConsoleOutput> messages;
	};
}