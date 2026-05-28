#include "UIElement.h"

#include "components/UIAspectRatioComponent.h"
#include "components/UIComponent.h"
#include "instances/services/RenderingService.h"

using namespace Nyanners::Instances;

namespace Nyanners::Scripting {
    static auto uiElement = ReflectionDescriptorRegistry::instance()->create_registrator([]() {
        Services::ReflectionService::create_descriptor("UIElement", {"Instance"}, {ReflectionInstanceFlags::NotCreatable})
            .add_property_chained<UIElement, int, &UIElement::get_zindex, &UIElement::set_zindex>("ZIndex", Integer)
            .add_property_chained<UIElement, glm::vec2, &UIElement::get_anchor_point, &UIElement::set_anchor_point>("AnchorPoint", Vector2)
            .add_property_chained<UIElement, glm::vec2, &UIElement::get_absolute_size>("AbsoluteSize", Vector2)
            .add_property_chained<UIElement, glm::vec2, &UIElement::get_absolute_position>("AbsolutePosition", Vector2)
            .add_property_chained<UIElement, glm::vec2, &UIElement::get_positionScale, &UIElement::set_positionScale>("PositionScale", Vector2)
            .add_property_chained<UIElement, glm::vec2, &UIElement::get_positionOffset, &UIElement::set_positionOffset>("PositionOffset", Vector2)
            .add_property_chained<UIElement, glm::vec2, &UIElement::get_sizeScale, &UIElement::set_sizeScale>("SizeScale", Vector2)
            .add_property_chained<UIElement, glm::vec2, &UIElement::get_sizeOffset, &UIElement::set_sizeOffset>("SizeOffset", Vector2);
    });
}

int UIElement::get_zindex() const {
    return zIndex;
}

void UIElement::set_zindex(const int newZindex) {
    zIndex = newZindex;
}

glm::vec2 UIElement::get_anchor_point() const {
    return this->anchorPoint;
}

void UIElement::set_anchor_point(const glm::vec2 &newAnchorPoint) {
    this->anchorPoint = newAnchorPoint;
}

glm::vec2 UIElement::get_positionScale() const {
    return this->positionScale;
}

void UIElement::set_positionScale(const glm::vec2 &newSizeScale) {
    this->positionScale = newSizeScale;
}

glm::vec2 UIElement::get_positionOffset() const {
    return this->positionOffset;
}

void UIElement::set_positionOffset(const glm::vec2 &newSizeOffset) {
    this->positionOffset = newSizeOffset;
}

glm::vec2 UIElement::get_sizeScale() const {
    return this->sizeScale;
}

void UIElement::set_sizeScale(const glm::vec2 &newSizeScale) {
    this->sizeScale = newSizeScale;
}

glm::vec2 UIElement::get_sizeOffset() const {
    return this->sizeOffset;
}

void UIElement::set_sizeOffset(const glm::vec2 &newSizeOffset) {
    this->sizeOffset = newSizeOffset;
}

glm::vec2 UIElement::get_absolute_size() const {
    const auto& viewportSize = Services::RenderingService::renderer->currentViewport->size;
    const auto scaleX = static_cast<int>(std::round(sizeScale.x * viewportSize.x));
    const auto scaleY = static_cast<int>(std::round(sizeScale.y * viewportSize.y));

    auto vector = glm::vec2(std::round(scaleX + sizeOffset.x), std::round(scaleY + sizeOffset.y));

    for (const auto& component : peek_at<UI::Components::UIAspectRatioComponent>("UIAspectRatioComponent")) {
        auto ptr = component.lock();

        if (ptr->active) {
            ptr->transform_scale(vector);
        }

        ptr.reset();
    }

    vector.x = std::round(vector.x);
    vector.y = std::round(vector.y);
    return vector;
}

glm::vec2 UIElement::get_absolute_position() const {
    const auto& viewportSize = Services::RenderingService::renderer->currentViewport->size;
    const auto positionX = static_cast<int>(std::round(positionScale.x * viewportSize.x));
    const auto positionY = static_cast<int>(std::round(positionScale.y * viewportSize.y));

    return glm::vec2(std::round(positionX + positionOffset.x), std::round(positionY + positionOffset.y));
}
