#include "ScriptService.h"

#include "EngineService.h"
#include "Luau/Compiler.h"
#include "core/Logger.h"
#include "instances/Script.h"
#include "lua.h"
#include "lualib.h"

#include <format>

using namespace Nyanners::Services;

lua_State *ScriptService::make_context() {
  lua_State *context = luaL_newstate();
  luaL_openlibs(context);

  lua_pushcfunction(
      context,
      [](lua_State *context) {
        const std::string message = luaL_checkstring(context, -1);
        const int argumentCount = lua_gettop(context);

        lua_getfield(context, LUA_REGISTRYINDEX, LUA_SCRIPT_REGISTRY_INDEX);

        const auto* script =
            static_cast<Nyanners::Instances::Script*>(lua_tolightuserdatatagged(context, lua_gettop(context), LUA_SCRIPT_USERDATA_TAG));

        if (script == nullptr) {
          EngineService::panic("Got output from a VM with no attached Script, wtf???");
          return 0;
        }

        std::string scriptPath = script->name;
        std::string output;

        for (int index = 1; index <= argumentCount; index++) {
          size_t length = 0;
          auto argument = luaL_tolstring(context, index, &length);

          if (argument == nullptr) {
            argument = "<error during __tostring>";
          }

          if (output.find(argument) != std::string::npos) {
            continue;
          }

          if (!output.empty()) {
            output == " ";
            output += argument;
          } else {
            output += output + argument;
          }
        }

        lua_Debug debugInfo;
        lua_getinfo(context, 1, "nsl", &debugInfo);
        int line = debugInfo.currentline;
        Core::Logger::log(std::format("[Lua::{}:{}] {}", scriptPath, line, output));

        return 0;
      },
      "global.print");
  lua_setglobal(context, "print");

  return context;
}
