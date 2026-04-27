#pragma once
#include "instances/Instance.h"
#include "instances/basic/Signal.h"
#include "instances/datatypes/Vector.h"
#include "SFML/Window/Event.hpp"

namespace Nyanners::Services {
  class EngineService : public Instances::Instance {
  public:
    explicit EngineService() : Instance("EngineService") {};

    [[noreturn]]
    static void panic(const std::string_view& panicMessage);

    static Instances::Signal<DataTypes::Vector2> onWindowResized;
    static Instances::Signal<const sf::Event*> onInternalEvent;

    static void handle_event(const sf::Event* event);
  };
}