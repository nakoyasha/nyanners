#pragma once
#include "instances/Instance.h"
#include "instances/drawable/Drawable.h"

namespace Nyanners::Services
{
    // TODO: add aux methods for ui
    class UIService : public Instances::Instance, public Instances::Drawable
    {
    public:
        UIService() : Instance("UIService") {};

        void add_child(const std::shared_ptr<Instance>& child) override;
        void draw(sf::RenderTarget &target) override;;
    private:
        std::vector<std::shared_ptr<Drawable>> drawables;
    };
}
