#include "Script.h"

#include "Application.h"
#include "Luau/Compiler.h"
#include "core/Logger.h"
#include "lua.h"
#include "services/ReflectionService.h"

#include <format>

using namespace Nyanners::Instances;


void Script::run_script() const {
  const char *mutableGlobals[] = {"game", "DataModel", "script", nullptr};

  // Compile Luau source to bytecode
  Luau::CompileOptions compileOptions;
  compileOptions.optimizationLevel = 2;
  compileOptions.debugLevel = 2;
  compileOptions.mutableGlobals = mutableGlobals;

  const std::string bytecode = Luau::compile(source, compileOptions);

  // Load the bytecode into the Lua state
  int result = luau_load(context, name.c_str(), bytecode.data(),
                         bytecode.size(), 0);

  // push game here, to keep the reference fresh
  // reflection_exposeInstanceToLua(context, Application::instance().dataModel);
  // lua_setglobal(context, "game");

  auto model = Application::instance()->currentModel;

  auto reflection = Services::ReflectionService::create_reflection(model,         {
            {
                .name = "Name",
                .readOnly = false,
                .value = model->name,
            },
          {
            .name = "ClassName",
            .readOnly = true,
            .value = model->baseName,
            }
        });
  Services::ReflectionService::reflect_class(context, reflection);
  lua_setglobal(context, "DataModel");

  if (result != LUA_OK) {
    Core::Logger::log(std::format("Failed to compile script {}, luau_load returned {}", name, result));
  }

  if (result == LUA_OK) {
    const int runResult = lua_pcall(context, 0, LUA_MULTRET, 0);

    if (runResult != LUA_OK) {
      std::string error = lua_tostring(context, -1);
      Core::Logger::log(error);
      return;
    } else {
      printf("result: %d\n", result);
    }

  }
}