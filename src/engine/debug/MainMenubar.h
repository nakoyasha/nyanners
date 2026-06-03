#pragma once
#include "Application.h"
#include "DebugWindow.h"

namespace Nyanners::Debug::UI {
	class MainMenubar : public Instances::DebugWindow {
	public:
		MainMenubar() : Instance("MenubarPanel") {};
		void draw() override;
	private:
		bool showTextureViewer = false;
	};
}