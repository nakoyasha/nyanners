#include "ScriptService.h"
#include "Application.h"
#include "EngineService.h"
#include "lua.h"
#include "lualib.h"
#include "Luau/Compiler.h"
#include "core/Logger.h"
#include "instances/Script.h"
#include "scripting/data/UserdataTags.h"
#include <format>

using namespace Nyanners::Services;

ScriptService::ScriptService() : Instance("ScriptService") {
}


lua_State *ScriptService::make_context() {
    lua_State *context = luaL_newstate();
    luaL_openlibs(context);


    lua_pushnumber(context, Core::LogLevel::Info);
    lua_pushcclosure(context, handle_lua_console, "global.print", 1);
    lua_setglobal(context, "print");
    lua_pushnumber(context, Core::LogLevel::Warning);
    lua_pushcclosure(context, handle_lua_console, "global.warn", 1);
    lua_setglobal(context, "warn");

    // const auto runService = Application::instance()->currentModel->get_service<RunService>("RunService");
    // ReflectionService::reflect_class(context, runService);
    // lua_pushcclosure(context, [](lua_State* context) {
    // 	const auto instance = ReflectionService::get_instance_from_context(context, lua_upvalueindex(1));
    //
    // 	if (instance->descriptor->className != "RunService") {
    // 		luaL_error(context, "Somehow didn't have RunService as upvalue????");
    // 	}
    //
    // 	const auto runService = std::dynamic_pointer_cast<RunService>(instance->pointer);
    // 	const auto time = runService->get_time_since_start();
    //
    // 	lua_pushnumber(context, time);
    //
    // 	return 1;
    // }, "global.tick", 1);

    return context;
}

int ScriptService::handle_lua_console(lua_State *context) {
    const int argumentCount = lua_gettop(context);
    const Core::LogLevel logLevel = static_cast<Core::LogLevel>(luaL_checknumber(context, lua_upvalueindex(1)));

    lua_getfield(context, LUA_REGISTRYINDEX, LUA_SCRIPT_REGISTRY_INDEX);

    const auto *script =
            static_cast<Nyanners::Instances::Script *>(lua_tolightuserdatatagged(
                context, lua_gettop(context), LUA_SCRIPT_USERDATA_TAG));

    if (script == nullptr) {
        EngineService::panic("Got output from a VM with no attached Script, wtf???");
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
            output = " ";
            output += argument;
        } else {
            output += output + argument;
        }
    }

    lua_Debug debugInfo;
    lua_getinfo(context, 1, "nsl", &debugInfo);
    int line = debugInfo.currentline;

    const auto message = std::format("[Lua::{}:{}] {}", scriptPath, line, output);

    Core::Logger::log_no_format(logLevel, message);

    return 0;
}
