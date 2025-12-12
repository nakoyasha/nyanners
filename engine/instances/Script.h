#pragma once

#include "Instance.h"
#include <raylib.h>
#include <string.h>

#include "lua.h"
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
        int luaIndex(lua_State* context, std::string keyName) override;
        int luaNewIndex(lua_State* context, std::string keyName, std::string keyValue) override;
        void update() override;

        void loadFromFile(std::string filePathToLoad);
        void loadFromString(std::string code);
        void runScript();
        static bool compileSource(lua_State* context, const std::string& source, const std::string& name);
        ~Script() override
        {
            lua_close(context);
        }

    private:
        lua_State* context;
        bool isRunning = false;
        std::string filePath = "";

        void initializeLua();

        static int executeScript(lua_State* context);
    };
}
}