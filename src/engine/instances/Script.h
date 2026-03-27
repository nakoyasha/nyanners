#pragma once
#include "Instance.h"
#include "services/ScriptService.h"

namespace Nyanners::Instances {
  class Script : public Instance {
  public:
    std::string source;
    lua_State* context;

    Script() : Instance("Script") {
      context = Services::ScriptService::make_context();
      lua_pushlightuserdatatagged(context, this, LUA_SCRIPT_USERDATA_TAG);
      lua_setfield(context, LUA_REGISTRYINDEX, LUA_SCRIPT_REGISTRY_INDEX);

    };

    void run_script() const;
  };
}