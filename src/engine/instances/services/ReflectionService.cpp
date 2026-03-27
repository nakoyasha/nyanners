#include "ReflectionService.h"
#include "lualib.h"
#include "core/Logger.h"

using namespace Nyanners::Services;
std::map<std::string, ReflectionClass> ReflectionService::classes;

ReflectionInstance* ReflectionService::get_instance_from_context(lua_State* context, const int id)
{
    auto* instance = static_cast<ReflectionInstance*>(lua_touserdatatagged(context, id, LUA_SCRIPT_INSTANCE_TAG));

    if (instance == nullptr)
    {
        throw std::runtime_error("Instance userdata is null or invalid userdata passed");
    }

    return instance;
}

void ReflectionService::reflect_class(lua_State* context, const std::shared_ptr<Instance>& instance)
{
    const auto descriptor = classes.find(instance->baseName);

    if (descriptor == classes.end())
        throw std::runtime_error(std::format("Class {} is missing a Reflection descriptor", instance->baseName));

    auto* selfUser = static_cast<ReflectionInstance*>(lua_newuserdatatagged(context, sizeof(ReflectionInstance), LUA_SCRIPT_INSTANCE_TAG));
    new (selfUser) ReflectionInstance {
        .pointer = instance.get(),
        .descriptor = &descriptor->second,
    };

    if (luaL_newmetatable(context, "instance"))
    {
        luaL_Reg sRegs[] = {
            {"__index", [](lua_State* context)
            {
                auto* instance = get_instance_from_context(context, 1);
                return instance_index(context, instance);
            }},
            {
                "__tostring", [](lua_State* context)
                {
                    lua_pushstring(context, "Instance");
                    return 1;
                },
            },
            {nullptr, nullptr}
        };

        luaL_register(context, nullptr, sRegs);
    }

    lua_setreadonly(context, -1, true);
    lua_setmetatable(context, -2);
}

ReflectionClass ReflectionService::create_reflection(const ReflectionClass& descriptor)
{
    auto existingClass = classes.find(descriptor.className);

    if (existingClass != classes.end())
    {
        return existingClass->second;
    }

    classes.insert(std::make_pair(descriptor.className, descriptor));

    return descriptor;
}

int ReflectionService::instance_index(lua_State* context, ReflectionInstance* instance)
{
    const std::string propertyName =
        luaL_checkstring(context, -1);

    if (instance == nullptr)
    {
        throw std::runtime_error("Instance userdata is null");
    }

    if (instance->pointer == nullptr)
    {
        throw std::runtime_error("Instance pointer is null");
    }

    for (const auto& property : instance->descriptor->properties)
    {
        if (propertyName == "Name")
        {
            lua_pushstring(context, instance->pointer->name.c_str());;
            return 1;
        } if (propertyName == "ClassName")
        {
            lua_pushstring(context, instance->pointer->baseName.c_str());;
            return 1;
        } if (propertyName == "find_first_child")
        {
            lua_pushcfunction(context, [](lua_State* context)
            {
                const auto* instance = get_instance_from_context(context, 1);
                const std::string childName = luaL_checkstring(context, -1);
                const auto child = instance->pointer->find_first_child<Instance>(childName);

                reflect_class(context, child);
                return 1;
            }, "find_first_child");

            return 1;
        }
        if (property.name == propertyName)
        {
            return property.get(instance->pointer, context);
        }

        luaL_error(context, std::format("{}::{} is an invalid property", instance->pointer->baseName, propertyName).c_str());
        return 0;
    }
}
