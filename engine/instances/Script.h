#pragma once

#include "Instance.h"
#include "stdlib.h"
#include <fstream>
#include <iostream>
#include <raylib.h>
#include <stdint.h>
#include <string.h>

#include "lua.h"
#include "luacode.h"
#include "lualib.h"

#include "lua/system.h"
#include "lua/utils.h"

#include "engine.h"

#include <filesystem>

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

        void update();

        void loadFromFile(std::string filePath);
        ~Script()
        {
            lua_close(context);
        }

    private:
        lua_State* context;

        void initializeLua();
        bool compileSource();

        int executeScript();
    };
}
}