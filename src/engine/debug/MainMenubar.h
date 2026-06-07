#pragma once
#include "Application.h"
#include "DebugWindow.h"

namespace Nyanners::Debug::UI {
	class MainMenubar : public Instances::DebugWindow {
	public:
		MainMenubar();
		void draw() override;
	private:
		bool showTextureViewer = false;
		bool showAboutWindow = false;
		std::shared_ptr<Resources::Texture> debugMenuIcon;
	};
}