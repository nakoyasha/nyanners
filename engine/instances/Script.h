#pragma once

#include "Instance.h"
#include <raylib.h>
#include <string.h>

#include "lua.h"

#include "lua/builtin/libInstance.h"
#include "lua/system.h"
#include "lua/utils.h"

#include "engine.h"

static const char* SCRIPT_NAME_GLOBAL_NAME = "_SUPER_SECRET_SCRIPT_NAME_I_SURE_HOPE_NO_ONE_SEES_THIS_AND_GETS_THE_SCRIPT_NAME_THIS_WAY";

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

        int callMethod(std::string method);
        int luaIndex(lua_State* context, std::string keyName);
        void update();

        void loadFromFile(std::string filePath);
        void loadFromString(std::string code);
        void runScript();
        ~Script()
        {
            lua_close(context);
        }

    private:
        lua_State* context;
        bool isRunning = false;

        void initializeLua();
        bool compileSource();

        int executeScript();
    };
}
}