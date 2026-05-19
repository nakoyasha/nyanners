#pragma once
#include "DebugWindow.h"
#include "instances/DataModel.h"
#include "instances/Script.h"
#include "instances/services/SelectionService.h"
#include "resources/Texture.h"

using namespace Nyanners::Resources;

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

namespace Nyanners::Debug::UI {
	class ExplorerPanel : public Nyanners::Instances::DebugWindow {

	public:
		ExplorerPanel();
		void render_vector(
		  const glm::vec3 &values, const ReflectionProperty& property, Instance *instance
		) const;
		void render_vector(
		  const glm::vec2 &values, const ReflectionProperty& property, Instance *instance
		) const;
		void render_instance(const std::shared_ptr<Instance> &instance);
		void draw() override;
		StringValueCache &get_or_make_string_cache(
		  const std::shared_ptr<Instance> &,
		  const std::string &property,
		  const std::string &value
		);

	private:
		void display_property(
		  const std::shared_ptr<Instance>& instance, const ReflectionProperty& property);
		static void display_color_property(
		  const DataTypes::Color3 &color,
		  const ReflectionProperty &property,
		  Instance *instance
		);

		const std::map<std::string_view, std::shared_ptr<Texture>> classIcons = {
		  {"World", Texture::create(Texture2D, "assets/textures/editor/world.png")},

		  {"Skybox",
		   Texture::create(Texture2D, "assets/textures/editor/Weather_clouds.png")},

		  {"TextLabel",
		   Texture::create(Texture2D, "assets/textures/editor/Style.png")},

		  {"ReflectionService",
		   Texture::create(Texture2D, "assets/textures/editor/reflection.png")},

		  {"ScriptService",
		   Texture::create(Texture2D, "assets/textures/editor/scriptlink.png")},

		  {"Folder",
		   Texture::create(Texture2D, "assets/textures/editor/folder.png")},

		  {"FrameCounter",
		   Texture::create(Texture2D, "assets/textures/editor/framecounter.png")},

		  {"Script",
		   Texture::create(Texture2D, "assets/textures/editor/script.png")},

		  {"UIService",
		   Texture::create(
		     Texture2D, "assets/textures/editor/Application_get.png"
		   )},

		  {"MeshPart",
		   Texture::create(Texture2D, "assets/textures/editor/Brick.png")},

		  {"DebugUIService",
		   Texture::create(Texture2D, "assets/textures/editor/Cog.png")},

		  {"EngineService",
		   Texture::create(Texture2D, "assets/textures/editor/Cog.png")},

		  {"Sound", Texture::create(Texture2D, "assets/textures/editor/Sound.png")},

		  {"IOService",
		   Texture::create(Texture2D, "assets/textures/editor/Disk_multiple.png")},

		  {"SelectionService",
		   Texture::create(
		     Texture2D, "assets/textures/editor/Silk_arrow_inout.png"
		   )},

		  {"RenderingService",
		   Texture::create(Texture2D, "assets/textures/editor/Photo_icon.png")},

		  {"InputService",
		   Texture::create(Texture2D, "assets/textures/editor/input.png")},

		  {"CommandBarPanel",
		   Texture::create(
		     Texture2D, "assets/textures/editor/Application_xp_terminal.png"
		   )},

		  {"Camera",
		   Texture::create(Texture2D, "assets/textures/editor/Photo_icon.png")},

		  {"Unknown",
		   Texture::create(Texture2D, "assets/textures/editor/exclamation.png")},
		};
		std::vector<StringValueCache> stringValueBuffers;
		std::shared_ptr<Nyanners::Instances::DataModel> activeDm;
		std::shared_ptr<Nyanners::Services::SelectionService> selectionService;
	};
}