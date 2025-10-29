#include "Script.h"

using namespace Nyanners::Instances;

std::string engine_readFile(std::string fileName)
{
    std::filesystem::path filePath = fileName;

    if (!std::filesystem::is_regular_file(filePath)) {
        Application::instance().panic("engine_readFile: Invalid path: " + fileName);
    }

    std::ifstream file(fileName);
    std::string result { std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>() };

    // file.close();
    return result;
}


bool Script::compileSource() {
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
        // Error occurred; error message is on top of the stack
        // lua_error(context);
        lua_errorHandler(context);
        return false;
    }
}

void Script::initializeLua() {
    context = lua_newstate(engine_allocator, NULL);
    luaL_openlibs(context);
    luabridge_defineBridgeMethod(context, "engine_DispatchNative",
        engine_LuaDispatchMessage);
    luabridge_defineBridgeMethod(context, "engine_DrawText", engine_LuaDrawText);
    luabridge_defineBridgeMethod(context, "engine_panic", engine_LuaEnginePanic);
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
    lua_setglobal(context, "_SUPER_SECRET_SCRIPT_NAME_I_SURE_HOPE_NO_ONE_SEES_THIS_AND_GETS_THE_SCRIPT_NAME_THIS_WAY");
    luabridge_defineBridgeMethod(context, "print", luabridge_receiveMessageFromLua);

    reflection_exposeInstanceToLua(context, this);
    lua_setglobal(context, "script");
}

int Script::executeScript() {
    if (compileSource()) {
        // on stack now, time to run!
        if (lua_pcall(context, 0, LUA_MULTRET, 0) != LUA_OK) {
            std::string error = lua_tostring(context, -1);
            std::cout << error << std::endl;
            return 0;
        } else {
            return 1;
        }
    }

    return 0;
}

int Script::callMethod(std::string method) {
    // lua_State* threadContext = lua_newthread(context);
    lua_getglobal(context, method.c_str());
    int callResult = lua_pcall(context, 0, 0, 0);

    // printf("callResult: %d\n", callResult);
    if (callResult != LUA_OK) {
        std::string error = lua_tostring(context, -1);
        std::cout << error << std::endl;
    }

    return callResult;
}

void Script::update() {
    this->callMethod("engine_update");
}

void Script::loadFromFile(std::string filePath) {
    std::string file = engine_readFile(filePath.c_str());

    source = file;
    m_name = filePath;

    // update the script name variable
    lua_pushstring(context, this->m_name.c_str());
    lua_setglobal(context, SCRIPT_NAME_GLOBAL);

    int result = executeScript();
    printf("result: %d\n", result);

    if (result == 0) {
        std::string errorMessage = lua_tostring(context, -1);
        lua_pushstring(context, (std::string("[ERROR] ") + errorMessage).c_str());
        luabridge_receiveMessageFromLua(context);
        // std::cout << std::string("") + this->m_name + std::string("!\n") + errorMessage);
    }

    // this->update();
}