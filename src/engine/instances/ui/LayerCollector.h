#pragma once
#include <map>

#include "UIElement.h"
#include "instances/Instance.h"
#include "instances/drawable/Drawable.h"

namespace Nyanners::Instances::UI {
    class LayerCollector : public Drawable, public Instance {
    public:
        LayerCollector() : Instance("LayerCollector") {};
        bool dirty = false;

        void add_child(const std::shared_ptr<Instance> &child) override;
        void remove_child(const std::shared_ptr<Instance> &child) override;

        void draw() override;
        void update(const float deltaTime) override;
    private:
        std::multimap<int, std::weak_ptr<UIElement>> elements {};

        void recalculate_multimap();;
    };
}
