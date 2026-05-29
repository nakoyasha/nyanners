#include "LayerCollector.h"
#include <ranges>

#include "instances/services/ReflectionService.h"
#include "scripting/reflections/ReflectionDescriptor.h"
#include "scripting/reflections/ReflectionDescriptorRegistry.h"

using namespace Nyanners::Instances::UI;

namespace Nyanners::Scripting {
    auto layerCollector = Reflection::ReflectionDescriptorRegistry::instance()->create_registrator([]() {
       Services::ReflectionService::create_descriptor("LayerCollector", {"Instance"})
        .add_constructor<LayerCollector>();
    });
}

void LayerCollector::add_child(const std::shared_ptr<Instance> &child) {
    Instance::add_child(child);

    for (const auto& drawable : renderableChildren) {
        if (const auto& element = std::dynamic_pointer_cast<UIElement>(drawable.lock())) {
            elements.insert(std::make_pair(element->zIndex, element));
        }
    }
}

void LayerCollector::remove_child(const std::shared_ptr<Instance> &child) {
    Instance::remove_child(child);

    // this is horrible, but whatever...
    if (const auto& element = std::dynamic_pointer_cast<UIElement>(child)) {
        const auto& range = elements.equal_range(element->zIndex);

        for (auto it = range.first; it != range.second;) {
            if (it->second.lock() == element) {
                it = elements.erase(it);
            } else {
                ++it;
            }
        }
    }
}

void LayerCollector::draw() {
    if (!this->active) {
        return;
    }

    for (auto it = elements.begin(); it != elements.end(); ++it) {
        std::shared_ptr<UIElement> ptr = it->second.lock();

        if (!ptr->active) {
            continue;
        }

        ptr->draw();
        if (it->first != ptr->zIndex) {
            dirty = true;
        }

        ptr.reset();

    }
}

void LayerCollector::update(const float deltaTime) {
    Instance::update(deltaTime);

    if (dirty) {
        recalculate_multimap();
        dirty = false;
    }
}

void LayerCollector::recalculate_multimap() {
    elements.clear();
    for (const auto& drawable : renderableChildren) {
        if (const auto& element = std::dynamic_pointer_cast<UIElement>(drawable.lock())) {
            elements.insert(std::make_pair(element->zIndex, element));
        }
    }
}
