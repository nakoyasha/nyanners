#pragma once
#include "DebugWindow.h"
#include "instances/Script.h"

namespace Nyanners::Debug::UI {
	class CommandBar : public Nyanners::Instances::DebugWindow {
	public:
		CommandBar();
		virtual void draw() override;
	private:
		std::string commandInput = "";
		std::shared_ptr<Nyanners::Instances::Script> script;
	};
}