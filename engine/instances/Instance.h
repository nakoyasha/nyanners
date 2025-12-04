#pragma once

#include "lua.h"
#include <iostream>
#include <string>
#include <vector>
#include "raylib.h"

namespace Nyanners {
namespace Instances {
    class Instance {

    public:
        Instance* m_parent = nullptr;

        std::string m_name;
        std::string m_className;
        std::vector<Instance*> children;

        Instance(const std::string className);
        virtual ~Instance();
        bool isA(const std::string className);

        virtual void update() { };
        virtual void draw() { };

        virtual int luaIndex(lua_State* context, std::string keyName);
        // string implementation
        virtual int luaNewIndex(lua_State* context, std::string keyName, std::string keyValue);
        // vector implementation
        virtual int luaNewIndex(lua_State* context, std::string keyName, Vector2 keyValue);
        virtual int luaDestroy(lua_State* context);

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