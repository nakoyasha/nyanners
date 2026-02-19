#include "Script.h"
#include "lua/builtin/libInstance.h"
#include "lua/builtin/libImmediate.h"
#include "lua/system.h"
#include "Luau/Require.h"
#include "Luau/Compiler.h"
#include "cassert"


using namespace Nyanners::Instances;


// shamelessly (not) shamelessly stolen from
// https://github.com/PhoenixWhitefire/PhoenixEngine/blob/main/src/impl/script/ScriptEngine.cpp#L1045
static void requireConfigInit(luarequire_Configuration *config) {
	config->is_require_allowed = [](lua_State *, void *, const char *) {
		return true;
	};
	config->reset = [](lua_State *, void *ctx, const char *chname) {
		static_cast<std::filesystem::path *>(ctx)->assign(chname);
		return NAVIGATE_SUCCESS;
	};
	config->jump_to_alias = [](lua_State *, void *, const char *) {
		return NAVIGATE_NOT_FOUND;
	};
	config->to_parent = [](lua_State*, void *ctx) {
		auto *currentPath = static_cast<std::filesystem::path *>(ctx);


		if (currentPath->has_parent_path()) {
			*currentPath = currentPath->parent_path();
			return NAVIGATE_SUCCESS;
		} else
			return NAVIGATE_NOT_FOUND;
	};
	config->to_child = [](lua_State* context, void *ctx, const char *name) {
		// if (scriptName == "<eval>") {
		// 	auto* currentPath = new std::filesystem::path();
		//
		// 	if (std::filesystem::exists(*currentPath / name)) {
		// 		auto* contextPath = static_cast<std::filesystem::path*>(ctx);
		// 		*contextPath /= name;
		//
		// 		delete currentPath;
		// 		return NAVIGATE_SUCCESS;
		// 	}
		//
		// 	delete currentPath;
		// 	return NAVIGATE_NOT_FOUND;
		// }

		auto *currentPath = static_cast<std::filesystem::path *>(ctx);
		if (!std::filesystem::exists(*currentPath / name))
			return NAVIGATE_NOT_FOUND;

		*currentPath /= name;
		return NAVIGATE_SUCCESS;
	};
	config->is_module_present = [](lua_State *, void *ctx) {
		auto *currentPath = static_cast<std::filesystem::path *>(ctx);

		if (std::filesystem::is_regular_file(*currentPath))
			return true;
		else
			return false;
	};
	config->get_chunkname = [](lua_State*, void *ctx, char *buffer, size_t bufferSize, size_t *outSize) {
		auto *currentPath = static_cast<std::filesystem::path *>(ctx);
		std::string strpath = currentPath->string();
		*outSize = strpath.size() + 1;

		if (bufferSize < strpath.size() + 1)
			return WRITE_BUFFER_TOO_SMALL;
		else {
			// memcpy(buffer + 1, strpath.data(), strpath.size());
			return WRITE_SUCCESS;
		}
	};
	config->get_loadname = config->get_chunkname; // TODO what's a loadname (the echo in the mirror)
	config->get_cache_key = config->get_chunkname;
	config->is_config_present = [](lua_State *, void *ctx) {
		return false;
	};
	config->get_config = [](lua_State *, void *ctx, char *buffer, size_t bufferSize, size_t *outSize) {
		return WRITE_FAILURE;
	};
	config->load = [](lua_State *L, void *ctx, const char * /* path */, const char *chname, const char *ldname) {
		auto *currentPath = static_cast<std::filesystem::path *>(ctx);

		// from `Luau/CLI/src/ReplRequirer.cpp` 13/08/2025
		// module needs to run in a new thread, isolated from the rest
		// note: we create moduleThread on main thread so that it doesn't inherit environment of L
		lua_State *globalThread = lua_mainthread(L);
		lua_State *moduleThread = lua_newthread(globalThread);
		lua_xmove(globalThread, L, 1);

		std::string contents = engine_readFile(currentPath->string());

		if (Script::compileSource(moduleThread, contents, chname)) {
			lua_pushstring(moduleThread, ldname);
			lua_setglobal(moduleThread, "_LOADNAME");

			int status = lua_resume(moduleThread, L, 0);
			Nyanners::Logger::log(std::format("Script::require status: {}", status));

			while (status == LUA_BREAK)
			{
				status = lua_resume(L, nullptr, 0);
			}

			if (status == LUA_OK) {
				if (lua_gettop(moduleThread) == 0) {
					lua_pushstring(moduleThread, "module must return a value");
				}
			} else if (status == LUA_YIELD) {
				lua_pushstring(moduleThread, "module can not yield");
			}
			else if (!lua_isstring(moduleThread, -1)) {
				lua_pushstring(moduleThread, "unknown error while running module");
			}
		} else {
			return 0;
		}

		// add moduleThread result to L stack
		lua_xmove(moduleThread, L, 1);

		// remove moduleThread thread from L stack
		lua_remove(L, -2);

		// added one value to L stack: module result
		return 1;
	};

	assert(!config->get_alias);
}

bool Script::compileSource(lua_State *context, const std::string &source, const std::string &name) {
	const char *mutableGlobals[] = {
		"game", "script",
		nullptr
	};

	// Compile Luau source to bytecode
	Luau::CompileOptions compileOptions;
	compileOptions.optimizationLevel = 2;
	compileOptions.debugLevel = 2;
	compileOptions.mutableGlobals = mutableGlobals;

	const std::string bytecode = Luau::compile(source, compileOptions);

	lua_pushlstring(context, name.data(), name.size());
	lua_setglobal(context, "_CHUNKNAME");

	// Load the bytecode into the Lua state
	int result = luau_load(context, name.c_str(), bytecode.data(), bytecode.size(), 0);
	Logger::log(std::format("luau_load result: {}", result));

	if (result == 0) {
		return true;
	} else {
		lua_errorHandler(context);
		return false;
	}
}

int scriptRequireImpl(lua_State *context) {
	int stackTop = lua_gettop(context);
	lua_getglobal(context, "_require");
	std::string modulePath = luaL_checkstring(context, stackTop);
	Nyanners::Logger::log(modulePath);

	lua_pushstring(context, modulePath.c_str());
	int callResult = lua_pcall(context, 1, LUA_MULTRET, 0);

	if (callResult != 0) {
		// Nyanners::Logger::log("Outer call result failed");
		auto error = lua_tostring(context, -1);
		lua_throwError(context, error);

		return 1;
	}

	luaL_checktype(context, -1, LUA_TFUNCTION);
	int innerResultCallType = lua_pcall(context, 0, LUA_MULTRET, 0);

	if (innerResultCallType != 0) {
		// Nyanners::Logger::log("Inner call result failed");
		auto error = lua_tostring(context, -1);
		lua_throwError(context, error);
		return 0;
	}

	Nyanners::Logger::log(std::format("requireResult: {}, innerCallResult: {}", callResult, innerResultCallType));
	// lua_pushnumber(context, callResult);
	luabridge_dumpstack(context);
	return 1;
}

void Script::initializeLua() {
	context = lua_newstate(engine_allocator, NULL);
	if (context == nullptr) {
		Application::instance().panic("OOM: lua_newstate (allocation failure)");
	}

	luaL_openlibs(context);
	libInstance_open(context);
	libImmediate_open(context);

	luabridge_defineBridgeMethod(context, "print", luabridge_receiveMessageFromLua);

	auto *requirePath = new std::filesystem::path(this->m_name);

	luaopen_require(
		context,
		requireConfigInit,
		requirePath
	);


	lua_pushstring(context, this->m_name.c_str());
	lua_setglobal(context, SCRIPT_NAME_GLOBAL_NAME);
	luabridge_defineBridgeMethod(context, "print", luabridge_receiveMessageFromLua);

	reflection_exposeInstanceToLua(context, this);
	lua_setglobal(context, "script");
}

int Script::executeScript(lua_State *context) {
	if (lua_pcall(context, 0, LUA_MULTRET, 0) != LUA_OK) {
		std::string error = lua_tostring(context, -1);
		lua_pushstring(context, error.c_str());
		return 0;
	} else {
		return 1;
	}

	return 0;
}

int Script::luaIndex(lua_State *context, std::string keyName) {
	if (keyName == "Source") {
		lua_pushstring(context, source.c_str());
		return 1;
	} else {
		return Instance::luaIndex(context, keyName);
	}
}

int Script::luaNewIndex(lua_State *context, std::string keyName, std::string keyValue) {
	if (keyName == "Source") {
		this->loadFromString(keyValue);
		return 0;
	} else {
		return Instance::luaNewIndex(context, keyName, keyValue);
	}
}

int Script::callMethod(std::string method) {
	lua_getglobal(context, method.c_str());
	int callResult = lua_pcall(context, 0, 0, 0);

	if (callResult != LUA_OK) {
		return LUA_ERRRUN;
	}

	return callResult;
}

void Script::update() {
	if (!isRunning) {
		isRunning = true;
		runScript();
	}
}

void Script::loadFromFile(const std::string filePathToLoad) {
	std::string file = engine_readFile(filePathToLoad.c_str());

	source = file;
	m_name = filePathToLoad;
	filePath = filePathToLoad;
}

void Script::loadFromString(const std::string code) {
	source = code;
	// m_name = "<eval>";
}

void Script::runScript() {
	lua_pushstring(context, this->m_name.c_str());
	lua_setglobal(context, SCRIPT_NAME_GLOBAL_NAME);

	// push game here, to keep the reference fresh
	reflection_exposeInstanceToLua(context, Application::instance().dataModel);
	lua_setglobal(context, "game");

	if (Script::compileSource(this->context, this->source, this->m_name)) {
		int result = executeScript(context);
		printf("result: %d\n", result);

		if (result == 0) {
			std::string errorMessage = lua_tostring(context, -1);
			lua_pushstring(context, errorMessage.c_str());

			luabridge_receiveMessageFromLua(context);
		}
	}
}
