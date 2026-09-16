#include "UIService.h"

#include "ReflectionService.h"
#include "scripting/reflections/ReflectionDescriptorRegistry.h"
using namespace Nyanners::Services;

namespace Nyanners::Scripting {
	[[maybe_unused]]
	static auto scriptServiceDescriptor = ReflectionDescriptorRegistry::instance()->create_registrator([]() {
		ReflectionService::create_descriptor("UIService", {"Instance"}, {ReflectionInstanceFlags::Service});
	});
}


//
// void UIService::add_child(const std::shared_ptr<Instance>& child)
// {
//     if (auto uiInstance = dynamic_pointer_cast<Drawable>(child)) {
//         drawables.push_back(uiInstance);
//     }
//
//     Instance::add_child(child);
// }

//
// void UIService::draw(sf::RenderTarget &target, const glm::mat4& mvp)
// {
//     for (const auto& drawable : drawables)
//     {
//         drawable->draw(target, mvp);
//     }
// }
