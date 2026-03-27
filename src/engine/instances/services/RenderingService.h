#pragma once
#include "instances/Instance.h"
#include "instances/drawable/Drawable.h"
#include "SFML/Graphics/RenderWindow.hpp"

namespace Nyanners::Services
{
    class RenderingService : public Instances::Instance
    {
        public:
        float deltaTime = 0.0f;
        int fps = 0.0f;
        sf::RenderWindow window;

        RenderingService() : Instances::Instance("RenderingService") {}

        void initialize(const sf::VideoMode size, const std::optional<std::string> windowTitle);
        void set_window_title(const std::string& newWindowTitle);
        bool is_window_open() const;
        void render(std::shared_ptr<Instances::Drawable> instanceToRender);
        void shutdown();

    };
}
