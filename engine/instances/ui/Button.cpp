#include "Button.h"

#include "core/Logger.h"
#include "engine.h"

using namespace Nyanners::Instances;

void Button::draw()
{
    DrawRectangle(position.x, position.y, size.x, size.y, WHITE);
    DrawText(m_text.c_str(), position.x / 2, position.y / 2, 16, BLACK);
}

Button::~Button()
{
    delete clicked;
}

void Button::update()
{
    Vector2Int screenSize = Application::instance().screenSize;
    Vector2 mousePosition = GetMousePosition();
    Rectangle bounds = { position.x, position.y, size.x, size.y };

    // Logger::log(std::format("{}, {}, w/h: {}, {}", bounds.x, bounds.y, bounds.width, bounds.height));

    if (CheckCollisionPointRec(mousePosition, bounds)) {
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            clicked->fire(mousePosition);
            Logger::log("we have been clicked!");
        }
    }
}
