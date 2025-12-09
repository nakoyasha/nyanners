#include "Reflection.h"
#include "lua/system.h"

#include "lua.h"
#include "luacode.h"
#include "lualib.h"
#include "core/Logger.h"
#include "core/Application.h"

using namespace Nyanners::Instances;

Instance* reflection_getInstance(lua_State* context, const int id)
{
    Instance* instance = *(Instance**)(lua_touserdata(context, id));
    return instance;
}

int reflection_metaIndex(lua_State* context)
{
    Instance* instance = reflection_getInstance(context);
    std::string property = lua_tostring(context, 2);

    if (instance == nullptr) {
        lua_throwError(context, "LuaBridge Error: Instance has gone off the stack? What?");
        return 0;
    }

    auto index = instance->properties.find(property);
    auto methodIndex = instance->methods.find(property);

    if (index != instance->properties.end()) {
        const Nyanners::Reflection::ReflectionProperty& prop = index->second;

        switch (prop.type) {
            case (Nyanners::Reflection::String): {
                auto* value = static_cast<std::string*>(prop.value);;
                lua_pushstring(context, value->c_str());
                break;
            };
            case (Nyanners::Reflection::Number): {
                auto value = static_cast<double*>(prop.value);
                lua_pushnumber(context, *value);
                break;
            };
            case (Nyanners::Reflection::ReflectionPropertyType::Instance): {
                Instance* indexedInstance = *static_cast<Instance**>(prop.value);

                if (!indexedInstance || Application::isInstanceValid(indexedInstance) == false) {
                    lua_pushnil(context);
                    break;
                }

                Nyanners::Logger::log("Indexing Instance");

                reflection_exposeInstanceToLua(context, indexedInstance);
                break;
            }
            default: {
                lua_throwError(context, std::format("Attempt to index invalid property {}", property));
                return 0;
            }
        }
    } else if (methodIndex != instance->methods.end()) {
        ReflectionMethod method = methodIndex->second;
        reflection_luaPushMethod(context, method);

        return 1;
    }
    else {
        // fallback
        Nyanners::Logger::log(std::format("{} could not be found in reflection falling back to manual indexing", property));
        return instance->luaIndex(context, property);
    }

    return 1;
}

int reflection_metaNewIndex(lua_State* context)
{
    Instance* instance = reflection_getInstance(context);
    std::string property = lua_tostring(context, 2);

    // throw nothing at lua to prevent a crash
    // todo: proper reference counting or something
    if (instance == NULL) {
        lua_pushnil(context);
        return 1;
    }

    // numbers must be checked first, as numbers can be __tostring'd
    // therefore, putting strings first would mean that is_string returns true,
    // it calls for the std::string implementation, and proceeds to die and confuse
    // the user.
    if (lua_isnumber(context, 3)) {
        const float value = lua_tonumber(context, 3);
        return instance->luaNewIndex(context, property, value);
    } else if (lua_isboolean(context, 3)) {
        const bool value = lua_toboolean(context, 3);
        return instance->luaNewIndex(context, property, value);
    }
    else if (lua_isstring(context, 3)) {
        std::string value = lua_tostring(context, 3);
        return instance->luaNewIndex(context, property, value);
    } else if (lua_isvector(context, 3)) {
        auto luaVector = lua_tovector(context , 3);
        Vector2 vector = {luaVector[0], luaVector[1]};
        return instance->luaNewIndex(context, property, vector);
    } else if (lua_isuserdata(context, 3)) {
        Instance* newInstance = reflection_getInstance(context, 3);
        return instance->luaNewIndex(context, property, newInstance);
    }
    else {
        lua_throwError(context, "__newindex for this type is not implemented.");
        return 0;
    }
}

int reflection_metaString(lua_State* context)
{
    Instance* instance = reflection_getInstance(context);

    lua_pushstring(context, instance->m_name.c_str());
    return 1;
}

void reflection_createInstanceMetatable(lua_State* context)
{
    if (luaL_newmetatable(context, "lua_instance")) {
        luaL_Reg sRegs[] = {
            { "__index", &reflection_metaIndex },
            { "__newindex", &reflection_metaNewIndex },
            { "__tostring", &reflection_metaString },
            { nullptr, nullptr }
        };
        luaL_register(context, nullptr, sRegs);
    }
}

void reflection_exposeInstanceToLua(lua_State* context, Instance* instance)
{
    Instance** self = (Instance**)lua_newuserdata(context, sizeof(Instance*));
    *self = instance;

    reflection_createInstanceMetatable(context);

    lua_setmetatable(context, -2);
};

int reflection_luaMethodWrapper(lua_State* context)
{
    auto* method = static_cast<ReflectionMethod*>(lua_touserdata(context, lua_upvalueindex(1)));

    if (method == nullptr) {
        std::cout << "reflection_luaMethodWrapper: method is nullptr" << std::endl;
        lua_throwError(context, "Reflection method GC'd while attempting to call");
        return 0;
    };

    int result = (*method)(context);

    return result;
}

int reflection_luaPushMethod(lua_State* context, ReflectionMethod method)
{
    // this is cursed, this is horror, but it works... somehow?
    // NOTE TO FUTURE SELF: sizeof(ReflectionMethod* <--- THE POINTER HERE CAUSED A CRASH!!!!! omg C++ IS EVIL)
    auto* ud = static_cast<ReflectionMethod*>(lua_newuserdata(context, sizeof(ReflectionMethod)));
    new (ud) ReflectionMethod(std::move(method));

    lua_pushcclosure(context, reflection_luaMethodWrapper, "Instance::_reflectionMethod", 1);

    return 1;
}

void reflection_luaPushValue(lua_State* context, const LuaValue& value)
{
    if (std::holds_alternative<std::string>(value)) {
        std::string cStr = std::get<std::string>(value);
        lua_pushlstring(context, cStr.c_str(), cStr.size());
        return;
    } else if (std::holds_alternative<int>(value)) {
        lua_pushnumber(context, std::get<int>(value));
        return;
    } else if (std::holds_alternative<double>(value)) {
        lua_pushnumber(context, std::get<double>(value));
        return;
    }
}

void reflection_luaPushStruct(lua_State* context, const std::map<std::string, LuaValue>& map)
{
    lua_newtable(context);
    int stackTop = lua_gettop(context);

    for (auto it = map.begin(); it != map.end(); ++it) {
        std::string key = it->first;
        LuaValue value = it->second; // <-- value is a LuaValue&

        lua_pushlstring(context, key.c_str(), key.size());
        reflection_luaPushValue(context, value);
        lua_settable(context, stackTop);
    }
}