#pragma once
#include "Drawable.h"
#include "core/Logger.h"
#include "instances/Instance.h"

namespace Nyanners::Instances {
  class TextLabel : public Instance, public Drawable {
    TextLabel() : Instance("TextLabel") {};

    void draw() override {
      Core::Logger::log("Drawing a TextLabel!");
    }
  };
}