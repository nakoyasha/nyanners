#pragma once
#include "DebugWindow.h"
#include "instances/Script.h"

namespace Nyanners::Debug::UI {
	class CommandBar : public Instances::DebugWindow {
	public:
		CommandBar();
		void draw() override;
	private:
		std::string commandInput = "";
		std::shared_ptr<Instances::Script> script;
	};
}