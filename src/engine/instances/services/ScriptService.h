#pragma once
#include "instances/Instance.h"
#include "lua.h"

#define LUA_SCRIPT_USERDATA_TAG 0x01
#define LUA_SCRIPT_REGISTRY_INDEX "current_script"

namespace Nyanners::Services {
  class ScriptService : public Instances::Instance {
  public:
    ScriptService() : Instance("ScriptService") {};

    static lua_State *make_context();
  };
}