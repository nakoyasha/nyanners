#include "ReflectionService.h"
#include "lualib.h"
#include "core/Logger.h"

using namespace Nyanners::Services;
std::map<std::string, ReflectionClass> ReflectionService::classes;

Nyanners::Instances::Instance* ReflectionService::get_instance_from_context(lua_State* context, const int id)
{
    Instance* instance = *static_cast<Instance**>(lua_touserdatatagged(context, id, LUA_SCRIPT_INSTANCE_TAG));

    return instance;
}


void ReflectionService::reflect_class(lua_State* context, const ReflectionClass& instance)
{
    auto** self = static_cast<Instance**>(lua_newuserdatatagged(context, sizeof(Instance*), LUA_SCRIPT_INSTANCE_TAG));
    *self = instance.pointer;

    if (luaL_newmetatable(context, "instance"))
    {
        luaL_Reg sRegs[] = {
            {"__index", [](lua_State* context)
            {
                try
                {
                    return ReflectionService::instance_index(context);
                } catch (std::runtime_error& e) {
                    luaL_error(context, e.what());
                    return 0;
                }
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
    lua_pushstring(context, "This metatable is locked");
    lua_setfield(context, -1, "__metatable");

    lua_setreadonly(context, -2, true);
    lua_setmetatable(context, -2);
}

ReflectionClass ReflectionService::create_reflection(const std::shared_ptr<Instance>& instance,
                                                     std::vector<ReflectionProperty> properties)
{
    auto existingClass = classes.find(instance->baseName);

    if (existingClass != classes.end())
    {
        return existingClass->second;
    }

    auto reflection = ReflectionClass{
        .className = instance->baseName,
        .base = "Instance",
        .isService = instance->baseName.find("Service") != std::string::npos,
        .pointer = instance.get(),
        .properties = std::move(properties)
    };

    classes.insert(std::pair{instance->baseName, reflection});

    return reflection;
}


int ReflectionService::instance_index(lua_State* context)
{
    auto* instance = get_instance_from_context(context, 1);
    const std::string propertyName =
        luaL_checkstring(context, -1);

    if (instance == nullptr)
    {
        throw std::runtime_error("Instance userdata is null");
    }

    auto reflectionMetadata = classes.find(instance->baseName);

    if (reflectionMetadata == classes.end())
    {
        throw std::runtime_error("Cannot index class with no reflection assigned");
    }

    for (const auto& property : reflectionMetadata->second.properties)
    {
        if (property.name == propertyName)
        {
            switch (property.type)
            {
            case (ReflectionPropertyType::Number):
                {
                    const float value = std::get<float>(property.value);
                    lua_pushnumber(context, value);
                    return 1;
                    break;
                }
            case (ReflectionPropertyType::String):
                {
                    const std::string value = std::get<std::string>(property.value);
                    lua_pushstring(context, value.c_str());
                    return 1;
                }
            case (ReflectionPropertyType::Method):
                {
                    const auto value = std::get<ReflectionMethod>(property.value);
                    auto* ud = static_cast<ReflectionMethod*>(lua_newuserdata(context, sizeof(ReflectionMethod)));
                    new (ud) ReflectionMethod(value);

                    lua_pushcclosure(context, [](lua_State* context)
                    {
                        const auto* method = static_cast<ReflectionMethod*>(lua_touserdata(context, lua_upvalueindex(1)));

                        if (method == nullptr) {
                            Core::Logger::log("reflection_luaMethodWrapper: method is nullptr");
                            luaL_error(context, "Reflection method GC'd while attempting to call");

                            return 0;
                        };

                        const int result = (*method)(context);

                        return result;
                    }, property.name.c_str(), 1);
                    return 1;
                }
            case (ReflectionPropertyType::UserData):
                {
                    const auto* value = std::get<void*>(property.value);

                    return 0;
                }
            default:
                throw std::runtime_error(std::format("Cannot retrieve property {}, as it's type is unknown",
                                                     propertyName));
                return 0;
            }
        }
    }

    return 0;
}
