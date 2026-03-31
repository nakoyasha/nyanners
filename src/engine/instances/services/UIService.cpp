#include "UIService.h"
using namespace Nyanners::Services;

void UIService::add_child(const std::shared_ptr<Instance>& child)
{
    if (auto uiInstance = dynamic_pointer_cast<Drawable>(child)) {
        drawables.push_back(uiInstance);
    }

    Instance::add_child(child);
}

void UIService::draw(sf::RenderTarget &target)
{
    for (const auto& drawable : drawables)
    {
        drawable->draw(target);
    }
}
