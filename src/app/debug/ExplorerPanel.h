#pragma once
#include "instances/DataModel.h"
#include "instances/Script.h"
#include "instances/debug/DebugWindow.h"
#include "instances/services/SelectionService.h"
#include "resources/Texture.h"

namespace TestApp::Panels {
	class ExplorerPanel : public Nyanners::Instances::DebugWindow {
		public:
		ExplorerPanel();
		  void render_instance(const std::shared_ptr<Instance> &instance);
		  void draw(const sf::RenderTarget& target) override;
	private:
		const Nyanners::Resources::Texture* workspaceIcon = Nyanners::Resources::Texture::create("assets/textures/editor/world.png");

		std::shared_ptr<Nyanners::Instances::Script> script;
		std::shared_ptr<Nyanners::Instances::DataModel> activeDm;
		std::shared_ptr<Nyanners::Services::SelectionService> selectionService;
	};
}