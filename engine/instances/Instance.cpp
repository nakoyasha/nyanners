#include "Instance.h"

#include "core/Logger.h"
#include "lua/reflection/Reflection.h"
#include "lua/system.h"

#include "lualib.h"
#include "core/Application.h"

using namespace Nyanners::Instances;

bool Instance::isA(std::string className)
{
    return this->m_className == className;
}

Instance* Instance::getChildByClass(const std::string className)
{
    for (Instance* inst : children) {
        if (inst->m_className == className) {
            return inst;
        }
    }

    return nullptr;
}

Instance* Instance::getChildByName(const std::string name)
{
    for (Instance* inst : children) {
        if (inst->m_name == name) {
            return inst;
        }
    }

    return nullptr;
}

void Instance::clearChild(Instance* instance)
{
    auto it = std::find_if(children.begin(), children.end(),
        [&](const auto& child) {
            return child == instance;
        });

    if (it != children.end())
        children.erase(it);
}

Instance::Instance(const std::string className)
{
    m_className = className;
    m_name = className;

    Application::addInstance(this);
}

void Instance::addChild(Instance* instance)
{
    // what
    if (instance == this) {
        Logger::log("Attempted to add this to this");
        return;
    }

    // if (auto child = this->getChildByName(instance->m_name)) {
    //     if (child->m_className == instance->m_className) {
    //         Logger::log("Attempted to set a child as parent");
    //         return;
    //     }
    // }

    // std::cout << instance->m_name << this->m_name << std::endl;

    if (instance->m_parent != nullptr) {
        instance->m_parent->clearChild(instance);
    }

    instance->m_parent = this;
    children.push_back(instance);
}

int Instance_luaIsA(lua_State* context)
{
    Instance* instance = reflection_getInstance(context);
    std::string checkClass = luaL_checkstring(context, -1);

    lua_pushboolean(context, instance->m_className == checkClass);
    return 1;
}

void Instance::luaDestroy()
{
    this->m_parent->clearChild(this);
    // TODO: figure out how to properly clear memory. currently we have to do this
    // setting to nil works.. i guess.
    // delete this;
}

int Instance::luaIndex(lua_State* context, const std::string keyName)
{
    if (keyName == "Name") {
        lua_pushstring(context, m_name.c_str());
        return 1;
    } else if (keyName == "ClassName") {
        lua_pushstring(context, m_className.c_str());
        return 1;
    } else if (keyName == "Parent") {
        if (m_parent != nullptr) {
            reflection_exposeInstanceToLua(context, m_parent);
        } else {
            lua_pushnil(context);
        }

        return 1;
    } else if (keyName == "IsA") {
        lua_pushcfunction(context, Instance_luaIsA, "Instance::IsA");
        return 1;
    } else if (keyName == "Destroy") {
        reflection_luaPushMethod(context, [this](lua_State* context) {
            this->luaDestroy();
            return 0;
        });
        return 1;
    } else {
        Instance* instance = this->getChildByName(keyName);

        if (instance != nullptr) {
            reflection_exposeInstanceToLua(context, instance);
            return 1;
        }

        lua_throwError(context, std::string("Attempt to index invalid property " + keyName).c_str());
        return 0;
    }
}

int Instance::luaNewIndex(lua_State* context, std::string keyName, std::string keyValue)
{
    if (keyName == "Name") {
        m_name = keyValue;
        return 0;
    } else {
        lua_throwError(context, std::string("Attempt to set invalid propertes " + keyName).c_str());
        return 0;
    }
}

int Instance::luaNewIndex(lua_State* context, std::string keyName, Vector2 keyValue)
{
    // a base instance has no positional properties
    lua_throwError(context, std::string("Attempt to set invalid property " + keyName).c_str());
    return 0;
}

int Instance::luaNewIndex(lua_State* context, std::string keyName, Instance* keyValue)
{
    if (keyName == "Parent") {
        keyValue->addChild(this);
    } else {
        lua_throwError(context, std::string("Attempt to set invalid property " + keyName).c_str());
    }

    return 0;
}

int Instance::luaNewIndex(lua_State* context, std::string keyName, float keyValue)
{
    // a base instance has no number properties
    lua_throwError(context, std::string("Attempt to set invalid property " + keyName).c_str());
    return 0;
}

int Instance::luaNewIndex(lua_State* context, std::string keyName, bool keyValue)
{
    // a base instance has no number properties
    lua_throwError(context, std::string("Attempt to set invalid property " + keyName).c_str());
    return 0;
}

Instance::~Instance()
{
    if (m_parent != nullptr) {
        m_parent->clearChild(this);
        m_parent = nullptr;
    }

    children.clear();

    for (Instance* child : children) {
        // why is this even here the hell
        if (child == nullptr) {
            continue;
        }

        delete child;
    }


    Application::removeInstance(this);
}