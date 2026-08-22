#include "Application.h"
#include "Script.h"
#include "lua.h"
#include "Luau/Compiler.h"
#include "core/Logger.h"
#include "scripting/LibDatatype.h"
#include "scripting/LibInstance.h"
#include "services/IOService.h"
#include "services/ReflectionService.h"
#include <format>
#include <ranges>

#include "scripting/reflections/ReflectionEnumRegistry.h"

using namespace Nyanners::Instances;

namespace Nyanners::Scripting {
	auto scriptRegistrator =
	  ReflectionDescriptorRegistry::instance()->create_registrator([]() {
		  Services::ReflectionService::create_descriptor("Script", {"Instance"})
		    .add_property_chained<
		      Script,
		      std::string,
		      &Script::get_file_path,
		      &Script::set_file>("Path", String)
		    .add_constructor<Script>();
	  });
}

Script::Script() : Instance("Script") {
	context = Services::ScriptService::make_context();
	lua_pushlightuserdatatagged(context, this, 0x02);
	lua_setfield(context, LUA_REGISTRYINDEX, "current_script");
}

// kept because im lazy to remove
void Script::initialize_script() {}

std::string Script::get_file_path() const {
	return this->filePath.string();
}

void Script::set_file(const std::string &scriptPath) {
	this->filePath = scriptPath;

	if (!Services::IOService::file_exists(scriptPath)) {
		throw std::runtime_error("Attempt to load non-existent script file");
	}

	std::string newSource = Services::IOService::read_file(scriptPath);
	this->set_source(newSource);
}

void Script::reload() {
	if (this->filePath.empty()) {
		Core::Logger::log_error("Script cannot be hot-reloaded with no file path");
		return;
	}

	this->isRunning = false;
	this->set_file(this->filePath.string());
	this->run_script();
}

void Script::set_source(std::string &source) {
	bytecode = std::move(Luau::compile(source, this->compileOptions));
	this->source = std::move(source);
}

void Script::run_script() {
	if (isRunning == true) {
		return;
	}

	const char *mutableGlobals[] = {"game", "DataModel", "script", nullptr};

	// Compile Luau source to bytecode
	// Luau::CompileOptions compileOptions;
	// compileOptions.optimizationLevel = 2;
	// compileOptions.debugLevel = 2;
	// compileOptions.mutableGlobals = mutableGlobals;

	// Load the bytecode into the Lua state
	int result =
	  luau_load(context, name.c_str(), bytecode.data(), bytecode.size(), 0);

	this->isRunning = true;

	auto model = Application::instance()->currentModel;
	Services::ReflectionService::reflect_class(context, model);
	lua_setglobal(context, "DataModel");

	Scripting::LibInstance::attach(context);
	lua_setglobal(context, "Instance");
	ReflectionEnumRegistry::instance().push_to_lua(context);
	Scripting::LibDatatype::attach(context);

	if (result != LUA_OK) {
		Core::Logger::log(
		  std::format(
		    "Failed to load script {}, luau_load returned {}", name, result
		  )
		);
		this->isRunning = false;
	} else {
		if (lua_pcall(context, 0, LUA_MULTRET, 0) != LUA_OK) {
			const std::string error = lua_tostring(context, -1);
			lua_Debug debugInfo;

			Core::Logger::log_error(error);
			luaL_traceback(context, context, nullptr, 1);

			const std::string traceback = lua_tostring(context, -1);
			lua_pop(context, -1);

			std::istringstream iterator(traceback);
			Core::Logger::log_debug("Stack Begin");
			for (std::string line; std::getline(iterator, line);) {
				if (line.find("[C]") != std::string::npos) {
					Core::Logger::log_debug("<engine internals>");
				} else {
					Core::Logger::log_debug(line);
				}
			}
			Core::Logger::log_debug("Stack End");
		} else {
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
