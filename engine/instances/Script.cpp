#include "Script.h"

using namespace Nyanners::Instances;

// TODO: actually implement require
struct ScriptRequireContext {
    Script* script;
};

bool Script::compileSource()
{
    // Compile Luau source to bytecode
    size_t bytecodeSize = 0;
    char* bytecode = luau_compile(source.c_str(), strlen(source.c_str()), NULL, &bytecodeSize);

    // Load the bytecode into the Lua state
    int result = luau_load(context, this->m_name.c_str(), bytecode, bytecodeSize, 0);

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

void Script::initializeLua()
{

    context = lua_newstate(engine_allocator, NULL);

    luaL_openlibs(context);
    //luabridge_defineBridgeMethod(context, "engine_DispatchNative",
      //  engine_LuaDispatchMessage);
    // luabridge_defineBridgeMethod(context, "engine_DrawText", engine_LuaDrawText);
    //luabridge_defineBridgeMethod(context, "engine_panic", engine_LuaEnginePanic);
    luabridge_defineBridgeMethod(context, "print", luabridge_receiveMessageFromLua);

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
    if (compileSource()) {
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

    this->callMethod("engine_update");
}

void Script::loadFromFile(const std::string filePath)
{
    std::string file = engine_readFile(filePath.c_str());

    source = file;
    m_name = filePath;
}

void Script::loadFromString(const std::string code)
{
    source = code;
    m_name = "<eval>";
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