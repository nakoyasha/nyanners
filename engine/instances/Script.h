#pragma once

#include "Instance.h"
#include "stdlib.h"
#include <fstream>
#include <iostream>
#include <raylib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "lua.h"
#include "luacode.h"
#include "lualib.h"

#include "../lua/system.h"
#include "../lua/utils.h"

static void* engine_allocator(void* ud, void* ptr, size_t osize, size_t nsize)
{
    (void)ud;
    (void)osize;

    if (nsize == 0) {
        free(ptr);
        return NULL;
    }
    return realloc(ptr, nsize);
}

std::string engine_readFile(std::string fileName)
{
    std::ifstream file(fileName);
    std::string result { std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>() };

    // file.close();
    return result;
}

namespace Nyanners {
namespace Instances {
    class Script : public Instance {
    public:
        std::string source;

        Script()
            : Instance("Script")
        {
            this->m_className = "Script";
            this->m_name = "Script";
            initializeLua();
        };

        int callMethod(std::string method)
        {
            lua_getglobal(context, method.c_str());
            int callResult = lua_pcall(context, 0, 0, 0);

            // printf("callResult: %d\n", callResult);
            if (callResult != LUA_OK) {
                std::string error = lua_tostring(context, -1);
                std::cout << error << std::endl;
            }

            return callResult;
        }

        void update()
        {
            this->callMethod("engine_update");
        }

        void loadFromFile(std::string filePath)
        {
            std::string file = engine_readFile(filePath.c_str());

            source = file;
            m_name = filePath;

            // update the script name variable
            lua_pushstring(context, this->m_name.c_str());
            lua_setglobal(context, "_SUPER_SECRET_SCRIPT_NAME_I_SURE_HOPE_NO_ONE_SEES_THIS_AND_GETS_THE_SCRIPT_NAME_THIS_WAY");

            int result = executeScript();
            printf("result: %d\n", result);

            if (result == 0) {
                std::string errorMessage = lua_tostring(context, -1);
                engine_panic("Script exception!\n" + errorMessage);
            }

            // this->update();
        }

        ~Script()
        {
            lua_close(context);
        }

    private:
        lua_State* context;

        void initializeLua()
        {
            context = lua_newstate(engine_allocator, NULL);
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
                engine_panic("OOM: lua_newstate (allocation failure)");
            }

            luaL_openlibs(context);
            lua_pushstring(context, this->m_name.c_str());
            lua_setglobal(context, "_SUPER_SECRET_SCRIPT_NAME_I_SURE_HOPE_NO_ONE_SEES_THIS_AND_GETS_THE_SCRIPT_NAME_THIS_WAY");
            luabridge_defineBridgeMethod(context, "print", luabridge_receiveMessageFromLua);
        }

        int executeScript()
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

                // on stack now, time to run!
                if (lua_pcall(context, 0, LUA_MULTRET, 0) != LUA_OK) {
                    std::string error = lua_tostring(context, -1);
                    std::cout << error << std::endl;
                    return 0;
                } else {
                    return 1;
                }
            } else {
                // Error occurred; error message is on top of the stack
                // lua_error(context);
                lua_errorHandler(context);
                return 0;
            }
        }
    };
}
}