#pragma once
#include "instances/Instance.h"
#include "lua.h"

#define LUA_SCRIPT_REGISTRY_INDEX "current_script"

namespace Nyanners::Services {
  class ScriptService : public Instances::Instance {
  public:
    ScriptService() : Instance("ScriptService") {};

    static lua_State *make_context();
  };
}