#pragma once
#include "instances/Script.h"
#include "instances/debug/DebugWindow.h"

namespace TestApp::Panels {
	class CommandBar : public Nyanners::Instances::DebugWindow {
	public:
		CommandBar();
		virtual void draw() override;
	private:
		std::string commandInput = "";
		std::shared_ptr<Nyanners::Instances::Script> script;
	};
}