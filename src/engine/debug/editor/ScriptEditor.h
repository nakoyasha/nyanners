#pragma once
#include "debug/DebugWindow.h"
#include "instances/Script.h"

namespace Nyanners::Debug {
	class ScriptEditor : public Instances::DebugWindow {
		public:
		ScriptEditor(std::shared_ptr<Instances::Script> script);

		void imgui_render() override;
	private:
		std::shared_ptr<Instances::Script> openScript;
	};
}
