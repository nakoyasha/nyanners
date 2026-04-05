#pragma once
#include "SFML/Window/Event.hpp"
#include "instances/Instance.h"
#include "instances/drawable/Drawable.h"

namespace Nyanners::Instances {
    class DebugWindow : virtual public Instance, public Drawable {
    public:
      DebugWindow();
    	virtual void imgui_render() {};
    };
}