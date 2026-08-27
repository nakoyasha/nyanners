#include "ScriptService.h"
#include "Application.h"
#include "EngineService.h"
#include "lua.h"
#include "lualib.h"
#include "Luau/Compiler.h"
#include <cassert>
#include "core/Logger.h"
#include "instances/Script.h"
#include <format>

#include "AssetService.h"

using namespace Nyanners::Services;
using namespace Nyanners::Scripting::Scheduler;

std::map<lua_State*, ScriptResumptionDate> ScriptService::scheduled;
lua_State* ScriptService::mainContext;

lua_State* ScriptService::active_context;

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
	return make_main_context();
}

void ScriptService::take_ownership_of_state(
  lua_State *context, Instances::Instance *owner
) {

}

std::shared_ptr<Nyanners::Instances::Script> ScriptService::load_script_file(const std::string &path) {
	if (AssetService::instance()->asset_file_exists(path)) {
		throw std::runtime_error("Script file does not exist");
	}

	const auto script = std::make_shared<Instances::Script>();
	script->set_file(path);
	script->initialize_script();

	return script;
}

Nyanners::Instances::Script* ScriptService::get_script_from_context(lua_State *context) {
	auto *script = static_cast<Instances::Script*>(lua_tolightuserdatatagged(context, -1, 0x02));

	if (script == nullptr || script->context == nullptr) {
		EngineService::panic("lua_State* with no Script attached");
	}

	return script;
}

lua_State * ScriptService::get_active_context() {
	assert(active_context != nullptr);
	return active_context;
}

void ScriptService::set_active_context(lua_State *context) {
	active_context = context;
}

void ScriptService::run_autorun() {
	const auto script = std::make_shared<Instances::Script>();
	const auto assets = AssetService::instance();
	const auto scriptsService = Application::instance()->currentModel->get_service<ScriptService>("ScriptService");

	try {
		if (assets->asset_file_exists("assets/autorun.luau")) {
			script->set_file("assets/autorun.luau");
		} else if (assets->asset_file_exists("autorun.luau")) {
			script->set_file("autorun.luau");
		} else {
			throw std::runtime_error("autorun script does not exist");
		}

		script->name = "autorun";
		script->initialize_script();
		script->run_script();

		scriptsService->add_child(script);
	} catch (std::runtime_error &e) {
		throw std::runtime_error(std::format("Failed to run autorun.luau: {}", e.what()));
	}
}

int ScriptService::handle_lua_console(lua_State *context) {
    const int argumentCount = lua_gettop(context);
    const auto logLevel = static_cast<Core::LogLevel>(luaL_checknumber(context, lua_upvalueindex(1)));

    lua_getfield(context, LUA_REGISTRYINDEX, "current_script");

    const auto script = get_script_from_context(context);
	lua_pop(context, 1);

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

	scheduled.emplace(context, schedule);
}

void ScriptService::update(const float deltaTime) {
	for (auto [context, schedule] : scheduled) {
		auto data = schedule;

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
