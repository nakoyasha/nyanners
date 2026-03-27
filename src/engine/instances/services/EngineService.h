#pragma once
#include "instances/Instance.h"

namespace Nyanners::Services {
  class EngineService : public Instances::Instance {
  public:
    explicit EngineService() : Instance("EngineService") {};

    [[noreturn]]
    static void panic(const std::string_view& panicMessage);
  };
}