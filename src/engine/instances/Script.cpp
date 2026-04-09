#include "Script.h"
#include "Application.h"
#include "lua.h"
#include "Luau/Compiler.h"
#include "core/Logger.h"
#include "scripting/LibInstance.h"
#include "scripting/data/UserdataTags.h"
#include "services/IOService.h"
#include "services/ReflectionService.h"
#include <format>
#include <ranges>

using namespace Nyanners::Instances;

void Script::initialize_script() {
  context = Services::ScriptService::make_context();
  lua_pushlightuserdatatagged(context, this, LUA_SCRIPT_USERDATA_TAG);
  lua_setfield(context, LUA_REGISTRYINDEX, LUA_SCRIPT_REGISTRY_INDEX);
}

void Script::set_file(const std::filesystem::path &scriptPath) {
  // doesn't require re-compilation
  if (this->filePath == scriptPath) {
    return;
  }

  this->filePath = scriptPath;

  if (!Services::IOService::file_exists(scriptPath)) {
    throw std::runtime_error("Attempt to load non-existent script file");
  }

  std::string newSource = Services::IOService::read_file(scriptPath);
  this->set_source(newSource);
}

void Script::set_source(std::string &source) {
  bytecode = std::move(Luau::compile(source, this->compileOptions));
  this->source = std::move(source);
}

void Script::run_script()
{
    if (isRunning == true)
    {
        return;
    }

    const char* mutableGlobals[] = {"game", "DataModel", "script", nullptr};

    // Compile Luau source to bytecode
    // Luau::CompileOptions compileOptions;
    // compileOptions.optimizationLevel = 2;
    // compileOptions.debugLevel = 2;
    // compileOptions.mutableGlobals = mutableGlobals;

    // Load the bytecode into the Lua state
    int result = luau_load(context, name.c_str(), bytecode.data(),
                           bytecode.size(), 0);

    this->isRunning = true;

    auto model = Application::instance()->currentModel;
    Services::ReflectionService::reflect_class(context, model);
    lua_setglobal(context, "DataModel");

		Scripting::LibInstance::attach(context);
		lua_setglobal(context, "Instance");

    if (result != LUA_OK)
    {
        Core::Logger::log(std::format("Failed to load script {}, luau_load returned {}", name, result));
        this->isRunning = false;
    } else
    {
        if (lua_pcall(context, 0, LUA_MULTRET, 0) != LUA_OK)
        {
            std::string error = lua_tostring(context, -1);
            Core::Logger::log(error);
        }
        else
        {
            this->isRunning = false;
            printf("result: %d\n", result);
        }
    }
}
void Script::set_active(const bool active) {
  if (!active) {
    this->wasPreviouslyRunning = this->isRunning;
    this->isRunning = false;
    lua_close(context);
  } else {
    this->initialize_script();

    if (wasPreviouslyRunning) {
      this->run_script();
    }
  }

  Instance::set_active(active);
}
