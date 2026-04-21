#pragma once
#include "instances/DataModel.h"
#include "instances/Script.h"
#include "instances/debug/DebugWindow.h"
#include "instances/services/SelectionService.h"
#include "resources/Texture.h"

struct StringValueCache {
	std::string_view value;
	std::shared_ptr<Nyanners::Instances::Instance> instance;
	std::array<char, 2048> buffer;
	bool operator==(const StringValueCache &ref) const {
		if (ref.value == value && ref.instance == instance) {
			return true;
		} else {
			return false;
		}
	}
};

namespace TestApp::Panels {
	class ExplorerPanel : public Nyanners::Instances::DebugWindow {

		public:
		ExplorerPanel();
			void render_vec3(
		  glm::vec3 &values, ReflectionProperty property, Instance *instance
		);
		  void render_instance(const std::shared_ptr<Instance> &instance);
		  void draw() override;
			StringValueCache& get_or_make_string_cache(
		    const std::shared_ptr<Instance> &, const std::string &property, const std::string &value
		  );
	private:
		const std::shared_ptr<Nyanners::Resources::Texture> workspaceIcon = Nyanners::Resources::Texture::create(TextureType::Texture2D, "assets/textures/editor/world.png");
		const std::shared_ptr<Nyanners::Resources::Texture> unknownIcon = Nyanners::Resources::Texture::create(TextureType::Texture2D, "assets/textures/editor/exclamation.png");

		std::vector<StringValueCache> stringValueBuffers;

		std::shared_ptr<Nyanners::Instances::Script> script;
		std::shared_ptr<Nyanners::Instances::DataModel> activeDm;
		std::shared_ptr<Nyanners::Services::SelectionService> selectionService;
	};
}