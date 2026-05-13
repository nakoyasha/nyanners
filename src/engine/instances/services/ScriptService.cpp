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
using namespace Nyanners::Scripting::Scheduler;

std::map<lua_State*, ScriptResumptionDate> ScriptService::scheduled;
lua_State* ScriptService::mainContext;

ScriptService::ScriptService() : Instance("ScriptService") {
}

lua_State* ScriptService::make_main_context() {
	lua_State *context = luaL_newstate();
	luaL_openlibs(context);

	lua_pushnumber(context, Core::LogLevel::Info);
	lua_pushcclosure(context, handle_lua_console, "global.print", 1);
	lua_setglobal(context, "print");
	lua_pushnumber(context, Core::LogLevel::Warning);
	lua_pushcclosure(context, handle_lua_console, "global.warn", 1);
	lua_setglobal(context, "warn");

	lua_pushcfunction(context, [](lua_State* context) {
		auto time = luaL_checknumber(context, -1);

		Core::Logger::log_debug(std::format("Halting script for {} seconds", static_cast<int>(time)));
		ScriptService::pause_context(context, sf::seconds(time));
		return lua_yield(context, 1);
	}, "wait");

	lua_setglobal(context, "wait");

	return context;
}

lua_State *ScriptService::make_context() {
		if (mainContext == nullptr) {
			mainContext = make_main_context();
		}

		auto context = lua_newthread(mainContext);
		return context;
}

void ScriptService::run_autorun() {
	auto script = std::make_shared<Instances::Script>();

	try {

		if (!IOService::file_exists("assets/autorun.luau")) {
			throw std::runtime_error("autorun script does not exist");
		}

		auto source = IOService::read_file("assets/autorun.luau");
		script->name = "autorun";
		script->set_source(source);
		script->initialize_script();
		script->run_script();
	} catch (std::runtime_error &e) {
		throw std::runtime_error(std::format("Failed to run autorun.luau: {}", e.what()));
	}
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

void ScriptService::pause_context(lua_State *context, const sf::Time& duration) {
	auto schedule = ScriptResumptionDate {sf::Clock(), std::move(duration)};
	schedule.timeLeft.start();

	scheduled.emplace(std::make_pair(context, schedule));
}

void ScriptService::update(const float deltaTime) {
	for (auto [context, schedule] : scheduled) {
		auto data = std::move(schedule);

		if (data.timeLeft.isRunning()) {
			data.timeLeft.start();
		}


		if (data.timeLeft.getElapsedTime().asSeconds() >= data.pausedFor.asSeconds()) {
			Core::Logger::log_debug("Resumptiong time up, resuming script");
			lua_resume(context, nullptr, 0);
			scheduled.erase(context);
		} else {
			Core::Logger::log_debug(std::format("Time left for context resumption: {}", data.timeLeft.getElapsedTime().asSeconds()));
		}
	}

	Instance::update(deltaTime);
}
