#pragma once
#include "lua.h"
#include "instances/Instance.h"

#define LUA_SCRIPT_REGISTRY_INDEX "current_script"

namespace Nyanners::Services {
  class ScriptService : public Instances::Instance {
  public:
    ScriptService();
    static lua_State *make_context();
    static int handle_lua_console(lua_State* context);
  };
}