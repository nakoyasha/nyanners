#include "RenderingService.h"

#include "instances/drawable/Drawable.h"
#include "SFML/Graphics/Font.hpp"

using namespace Nyanners::Services;

void RenderingService::initialize(const sf::VideoMode size, const std::optional<std::string> windowTitle)
{
    if (windowTitle.has_value())
    {
        window = sf::RenderWindow(size, windowTitle.value());
    } else
    {
        window = sf::RenderWindow(size, "Nyanners");
    }
}

void RenderingService::render(std::shared_ptr<Instances::Drawable> instanceToRender)
{
    while (const std::optional event = window.pollEvent())
    {
        // Close window: exit
        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }
    }

    window.clear();
    instanceToRender->draw(this->window);
    window.display();
}

void RenderingService::set_window_title(const std::string& newWindowTitle)
{
    window.setTitle(newWindowTitle);
}

void RenderingService::set_fps_limit(const unsigned int limit)
{
    this->window.setFramerateLimit(limit);
}

bool RenderingService::is_window_open() const
{
    return window.isOpen();
}

void RenderingService::shutdown()
{
    this->window.close();
}

