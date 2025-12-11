#include "Script.h"
#include "lua/builtin/libInstance.h"
#include "lua/system.h"
#include "Luau/Require.h"

#include "cassert"

using namespace Nyanners::Instances;

// TODO: actually implement require
static void requireConfigInit(luarequire_Configuration* config)
{
	config->is_require_allowed = [](lua_State*, void*, const char*)
		{
			return true;
		};
	config->reset = [](lua_State*, void* ctx, const char* chname)
		{
			// chunkname is prefixed with @
			//assert(chname[0] == '@');
			((std::filesystem::path*)ctx)->assign(chname);
			return NAVIGATE_SUCCESS;
		};
	config->jump_to_alias = [](lua_State*, void*, const char*)
		{
			return NAVIGATE_NOT_FOUND;
		};
	config->to_parent = [](lua_State*, void* ctx)
		{
			std::filesystem::path* curpath = (std::filesystem::path*)ctx;

			if (curpath->has_parent_path())
			{
				*curpath = curpath->parent_path();
				return NAVIGATE_SUCCESS;
			}
			else
				return NAVIGATE_NOT_FOUND;
		};
	config->to_child = [](lua_State*, void* ctx, const char* name)
		{
			std::filesystem::path* curpath = (std::filesystem::path*)ctx;
			if (!std::filesystem::exists(*curpath / name))
				return NAVIGATE_NOT_FOUND;

			*curpath /= name;
			return NAVIGATE_SUCCESS;
		};
	config->is_module_present = [](lua_State*, void* ctx)
		{
			std::filesystem::path* curpath = (std::filesystem::path*)ctx;

			if (std::filesystem::is_regular_file(*curpath))
				return true;
			else
				return false;
		};
	config->get_chunkname = [](lua_State*, void* ctx, char* buffer, size_t bufferSize, size_t* outSize)
		{
			std::filesystem::path* curpath = (std::filesystem::path*)ctx;
			std::string strpath = curpath->string();
			*outSize = strpath.size() + 1;

			if (bufferSize < strpath.size() + 1)
				return WRITE_BUFFER_TOO_SMALL;
			else
			{
				// memcpy(buffer + 1, strpath.data(), strpath.size());
				return WRITE_SUCCESS;
			}
		};
	config->get_loadname = config->get_chunkname; // TODO what's a loadname
	config->get_cache_key = config->get_chunkname;
	config->is_config_present = [](lua_State*, void* ctx) {
		return false;
	};
	config->get_config = [](lua_State*, void* ctx, char* buffer, size_t bufferSize, size_t* outSize)
		{
			return luarequire_WriteResult::WRITE_FAILURE;
		};
	config->load = [](lua_State* L, void* ctx, const char* /* path */, const char* chname, const char* ldname)
		{
			std::filesystem::path* curpath = (std::filesystem::path*)ctx;

			// from `Luau/CLI/src/ReplRequirer.cpp` 13/08/2025

			// module needs to run in a new thread, isolated from the rest
			// note: we create ML on main thread so that it doesn't inherit environment of L
			lua_State* GL = lua_mainthread(L);
			lua_State* ML = lua_newthread(GL);
			lua_xmove(GL, L, 1);

			// new thread needs to have the globals sandboxed
			// luaL_sandboxthread(ML);

			bool readSuccess = true;
			std::string contents = engine_readFile(curpath->string());

			if (!readSuccess)
			{
				lua_pop(GL, 1);
				luaL_error(L, "%s", contents.c_str());
				return 1;
			}

			if (Script::compileSource(ML, contents, chname) == 0)
			{
				lua_pushstring(ML, ldname);
				lua_setglobal(ML, "_LOADNAME");

				int status = lua_resume(ML, L, 0);

				if (status == LUA_OK)
				{
					if (lua_gettop(ML) == 0)
						lua_pushstring(ML, "module must return a value");
				}
				else if (status == LUA_YIELD)
					lua_pushstring(ML, "module can not yield");

				else if (!lua_isstring(ML, -1))
					lua_pushstring(ML, "unknown error while running module");
			}

			// add ML result to L stack
    		lua_xmove(ML, L, 1);
    		if (lua_isstring(L, -1))
    		    lua_error(L);

    		// remove ML thread from L stack
    		lua_remove(L, -2);

			// printf("type: %s\n", ML, lua_gettop(ML)));
			luabridge_dumpstack(ML);
    		// added one value to L stack: module result
			return 1;
		};

	assert(!config->get_alias);
}


bool Script::compileSource(lua_State* context, const std::string source, const std::string name)
{
    // Compile Luau source to bytecode
    size_t bytecodeSize = 0;
    char* bytecode = luau_compile(source.c_str(), strlen(source.c_str()), NULL, &bytecodeSize);

    // Load the bytecode into the Lua state
    int result = luau_load(context, name.c_str(), bytecode, bytecodeSize, 0);

    printf("luau_load result: %d\n", result);

    // Free the compiled bytecode
    free(bytecode);

    if (result == 0) {
        return true;
    } else {
        lua_errorHandler(context);
        return false;
    }
}

int scriptRequireImpl(lua_State* context) {
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
	//
	//
	//
	//
	//
}

void Script::initializeLua()
{

    context = lua_newstate(engine_allocator, NULL);

    if (context == nullptr) {
        Application::instance().panic("Failure initializing lua_State*");
    }

    luaL_openlibs(context);
    //luabridge_defineBridgeMethod(context, "engine_DispatchNative",
      //  engine_LuaDispatchMessage);
    // luabridge_defineBridgeMethod(context, "engine_DrawText", engine_LuaDrawText);
    //luabridge_defineBridgeMethod(context, "engine_panic", engine_LuaEnginePanic);
    luabridge_defineBridgeMethod(context, "print", luabridge_receiveMessageFromLua);

	auto* requirePath = new std::filesystem::path(this->m_name);

	luarequire_pushrequire(
		this->context,
		requireConfigInit,
		requirePath
	);

	lua_setglobal(context, "_require");

	// push our own
	lua_pushcfunction(context, scriptRequireImpl, "Script::require");
	lua_setglobal(context, "require");

    lua_createtable(context, 0, 0);
    int top = lua_gettop(context);
    lua_pushstring(context, "version");
    lua_pushstring(context, "v0.0.1a");
    lua_settable(context, -3);
    lua_pushstring(context, "quit");
    lua_pushcfunction(context, engine_LuaEngineExit, "engine");
    lua_settable(context, -3);
    lua_setglobal(context, "engine");

    if (!context) {
        Application::instance().panic("OOM: lua_newstate (allocation failure)");
    }

    lua_pushstring(context, this->m_name.c_str());
    lua_setglobal(context, SCRIPT_NAME_GLOBAL_NAME);
    luabridge_defineBridgeMethod(context, "print", luabridge_receiveMessageFromLua);

    reflection_exposeInstanceToLua(context, this);
    lua_setglobal(context, "script");

    lua_newtable(context);
    int topIndex = lua_gettop(context);

    lua_pushcfunction(context, &libInstance_new, "Instance.new");
    lua_setfield(context, topIndex, "new");
    lua_setglobal(context, "Instance");

}

int Script::executeScript()
{
    if (Script::compileSource(this->context, this->source, this->m_name)) {
        // on stack now, time to run!
        if (lua_pcall(context, 0, LUA_MULTRET, 0) != LUA_OK) {
            std::string error = lua_tostring(context, -1);
            // std::cout << error << std::endl;
            return 0;
        } else {
            return 1;
        }
    }

    return 0;
}

int Script::luaIndex(lua_State* context, std::string keyName)
{
    if (keyName == "Source") {
        lua_pushstring(context, source.c_str());
        return 1;
    } else {
        return Instance::luaIndex(context, keyName);
    }
}

int Script::luaNewIndex(lua_State* context, std::string keyName, std::string keyValue)
{
    if (keyName == "Source") {
        this->loadFromString(keyValue);
        return 0;
    } else {
        return Instance::luaNewIndex(context, keyName, keyValue);
    }
}

int Script::callMethod(std::string method)
{
    lua_getglobal(context, method.c_str());
    int callResult = lua_pcall(context, 0, 0, 0);

    if (callResult != LUA_OK) {
        return LUA_ERRRUN;
    }

    return callResult;
}

void Script::update()
{
    if (!isRunning) {
        isRunning = true;
        runScript();
    }
}

void Script::loadFromFile(const std::string filePathToLoad)
{
    std::string file = engine_readFile(filePathToLoad.c_str());

    source = file;
    m_name = filePathToLoad;
	filePath = filePathToLoad;
}

void Script::loadFromString(const std::string code)
{
    source = code;
    // m_name = "<eval>";
}

void Script::runScript()
{
    lua_pushstring(context, this->m_name.c_str());
    lua_setglobal(context, SCRIPT_NAME_GLOBAL_NAME);

    // push game here, to keep the reference fresh
    reflection_exposeInstanceToLua(context, Application::instance().dataModel);
    lua_setglobal(context, "game");

    int result = executeScript();
    printf("result: %d\n", result);

    if (result == 0) {
        std::string errorMessage = lua_tostring(context, -1);
        lua_pushstring(context, errorMessage.c_str());

        luabridge_receiveMessageFromLua(context);
    }
}