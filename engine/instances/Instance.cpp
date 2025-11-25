#include "Instance.h"
#include "lua/reflection/Reflection.h"
#include "lua/system.h"

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
}

void Instance::addChild(Instance* instance)
{
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

int Instance::luaDestroy(lua_State* context)
{
    this->m_parent->clearChild(this);
    delete this;
    return 0;
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
        bool isNotNull = m_parent != nullptr;
        if (isNotNull) {
            reflection_exposeInstanceToLua(context, m_parent);
        } else {
            lua_pushnil(context);
        }
        // lua_pushstring(context, "Instance.Parent is unimplemented");
        // lua_error(context);
        return 1;
    } else if (keyName == "IsA") {
        lua_pushcfunction(context, Instance_luaIsA, "Instance::IsA");
        return 1;
    } else if (keyName == "Destroy") {
        // lua_pushcfunction(context, Instance_luaDestroy, "Instance::Destroy");
        // reflection_luaPushMethod(context, std::bind(&Instance::luaDestroy, std::placeholders::_1, std::placeholders::_2));
        reflection_luaPushMethod(context, [this](lua_State* context) {
            // std::cout << "hi im haruka and i luvv emilyyy" << std::endl;
            this->luaDestroy(context);
            return 1;
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
        lua_throwError(context, std::string("Attempt to set invalid property " + keyName).c_str());
        return 0;
    }
}

Instance::~Instance()
{
    if (m_parent != nullptr) {
        m_parent->clearChild(this);
        m_parent = nullptr;
    }

    for (Instance* child : children) {
        delete child;
    }

    children.clear();
}