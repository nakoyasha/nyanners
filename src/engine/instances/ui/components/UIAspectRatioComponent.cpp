#include "UIAspectRatioComponent.h"

#include "instances/services/ReflectionService.h"
#include "scripting/reflections/ReflectionDescriptorRegistry.h"

using namespace Nyanners::UI::Components;

namespace Nyanners::Scripting {
    static auto aspectRatioComponent = ReflectionDescriptorRegistry::instance()->create_registrator([]() {
       Services::ReflectionService::create_descriptor("UIAspectRatioComponent", {"UIComponent", "Instance"})
        .add_property_chained<UIAspectRatioComponent, double, &UIAspectRatioComponent::get_aspect_ratio, &UIAspectRatioComponent::set_aspect_ratio>("AspectRatio", Number)
        .add_constructor<UIAspectRatioComponent>();
    });
}

void UIAspectRatioComponent::transform_scale(glm::vec2 &size) {
    auto original = size.x / size.y;

    if (original > targetAspectRatio) {
        size.x = size.y * targetAspectRatio + 0.5;
    } else {
        size.y = size.x * targetAspectRatio + 0.5;
    }
}

double UIAspectRatioComponent::get_aspect_ratio() const {
    return this->targetAspectRatio;
}

void UIAspectRatioComponent::set_aspect_ratio(const double newAspectRatio) {
    this->targetAspectRatio = newAspectRatio;
}
