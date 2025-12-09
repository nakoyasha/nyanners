#pragma once

#include "lua.h"
#include <iostream>
#include <string>
#include <vector>

#include "lualib.h"
#include "raylib.h"

#include "lua/reflection/ReflectionTypes.h"

namespace Nyanners {
namespace Instances {
    class Instance {

    public:
        Instance* m_parent = nullptr;

        std::string m_name;
        std::string m_className;
        std::vector<Instance*> children;

        Reflection::ReflectionProperties properties = {
            {"Name", {
                Reflection::ReflectionPropertyType::String,
                &m_name
            }},
            {"Parent", {
                Reflection::ReflectionPropertyType::Instance,
                &m_parent
            }}
        };

        Reflection::ReflectionMethods methods = {
            {"IsA", [this](lua_State* context) {
                const std::string className = luaL_checkstring(context, -1);
                lua_pushboolean(context, this->isA(className));
                return 1;
            }}
        };

        Instance(const std::string className);
        virtual ~Instance();
        bool isA(const std::string className);

        virtual void update() {
            for (auto instance : children) {
                instance->draw();
            }
        };

        virtual void draw() {
            for (auto instance : children) {
                instance->draw();
            }
        };

        virtual int luaIndex(lua_State* context, std::string keyName);
        // string implementation
        virtual int luaNewIndex(lua_State* context, std::string keyName, std::string keyValue);
        // vector implementation
        virtual int luaNewIndex(lua_State* context, std::string keyName, Vector2 keyValue);
        // instance implementation
        virtual int luaNewIndex(lua_State* context, std::string keyName, Instance* keyValue);
        // number implementation
        virtual int luaNewIndex(lua_State* context, std::string keyName, float keyValue);
        // bool implementation
        virtual int luaNewIndex(lua_State* context, std::string keyName, bool keyValue);
        virtual void luaDestroy();

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
