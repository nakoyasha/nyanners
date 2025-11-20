#pragma once

#include "lua.h"
#include "lualib.h"
#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>

namespace Nyanners {
namespace Instances {
    class Instance {

    public:
        std::string m_name;
        std::string m_className;

        Instance* m_parent = nullptr;
        std::vector<Instance*> children;

        Instance(const std::string className);
        ~Instance();
        bool isA(const std::string className);

        virtual void update() { };
        virtual void draw() { };

        virtual int luaIndex(lua_State* context, std::string keyName);
        // string implementation
        int luaNewIndex(lua_State* context, std::string keyName, std::string keyValue);
        int luaDestroy(lua_State* context);

        virtual bool isUI()
        {
            return false;
        }

        void addChild(Instance* instance);
        Instance* getChildByClass(const std::string className);
        Instance* getChildByName(const std::string className);
        // TODO: implement like actual children lmao
        void clearChild(Instance* instance);
    };
}
}