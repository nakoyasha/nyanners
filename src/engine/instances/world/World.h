#pragma once
#include "instances/Instance.h"

namespace Nyanners::Services {
  class World : public Instances::Instance {
  public:
    // TODO: physics so World has an actual usecase
    World() : Instance("World") {};
  };
}
