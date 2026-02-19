#include "system.h"
#include "core/Application.h"
#include "core/Logger.h"
#include <fstream>

using Method = void (Nyanners::Instances::DataModel::*)(int);

void lua_throwError(lua_State* context, std::string error)
{
    lua_pushstring(context, error.c_str());
    lua_error(context);
}

void luabridge_defineBridgeMethod(lua_State* context, std::string name,
    lua_CFunction method)
{
    lua_pushcfunction(context, method, name.c_str());
    lua_setglobal(context, name.c_str());
};

int luabridge_receiveMessageFromLua(lua_State* context)
{
    int argumentCount = lua_gettop(context);
    lua_getglobal(context, SCRIPT_NAME_GLOBAL);
    std::string scriptPath = lua_tostring(context, argumentCount + 1);
    std::string output = "";

    for (int index = 1; index <= argumentCount; index++) {
        size_t length = 0;
        auto argument = luaL_tolstring(context, index, &length);

        if (argument == nullptr) {
            argument = "<error during __tostring>";
        }

        if (output.find(argument) != std::string::npos) {
            continue;
        }

        if (output != "") {
            output = output + " " + argument;
        } else {
            output = output + argument;
        }
    }

    lua_Debug debugInfo;
    lua_getinfo(context, 1, "nsl", &debugInfo);
    int line = debugInfo.currentline;
    Nyanners::Logger::log(std::format("[Lua::{}:{}] {}", scriptPath, line, output));

    return 0;
}

int engine_LuaDrawText(lua_State* context)
{
    std::string text = luaL_checkstring(context, 1);
    DrawText(text.c_str(), 190, 200, 20, LIGHTGRAY);
    return 0;
}

int engine_LuaEnginePanic(lua_State* context)
{
    std::string text = luaL_checkstring(context, 1);
    lua_getglobal(context, SCRIPT_NAME_GLOBAL);
    std::string scriptName = lua_tostring(context, 2);

    Application::instance().panic(std::string("Panic encountered while executing ") + scriptName + ":\n" + text);
    return 0;
}

int engine_LuaEngineExit(lua_State* context)
{
    Application::instance().stop();
    return 0;
}

std::string engine_readFile(std::string fileName)
{
    std::filesystem::path filePath = fileName;

    if (!std::filesystem::is_regular_file(filePath)) {
        Application::instance().panic("engine_readFile: Invalid path: " + fileName);
    }

    std::ifstream file(fileName);
    std::string result { std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>() };

    return result;
}

void luabridge_dumpstack(lua_State* L)
{
    int top = lua_gettop(L);
    for (int i = 1; i <= top; i++) {
        printf("%d\t%s\t", i, luaL_typename(L, i));
        switch (lua_type(L, i)) {
        case LUA_TNUMBER:
            printf("%g\n", lua_tonumber(L, i));
            break;
        case LUA_TSTRING:
            printf("%s\n", lua_tostring(L, i));
            break;
        case LUA_TBOOLEAN:
            printf("%s\n", (lua_toboolean(L, i) ? "true" : "false"));
            break;
        case LUA_TNIL:
            printf("%s\n", "nil");
            break;
        default:
            printf("%p\n", lua_topointer(L, i));
            break;
        }
    }
}

int engine_LuaDispatchMessage(lua_State* context)
{
    std::string message = luaL_checkstring(context, 1);

    if (message == "Engine.Exit") {
        std::cout << "Lua requested engine exit" << std::endl;
        Application::instance().stop();
    } else {
        lua_throwError(context, "Invalid dispatch message");
    }

    return 0;
};