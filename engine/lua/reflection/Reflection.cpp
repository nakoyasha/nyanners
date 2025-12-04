#include "Reflection.h"
#include "lua/system.h"

using namespace Nyanners::Instances;

Instance* reflection_getInstance(lua_State* context)
{
    Instance* instance = *(Instance**)(lua_touserdata(context, 1));
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

    return instance->luaIndex(context, property);
}

int reflection_metaNewIndex(lua_State* context)
{
    Instance* instance = reflection_getInstance(context);
    std::string property = lua_tostring(context, 2);

    if (lua_isstring(context, 3)) {
        std::string value = lua_tostring(context, 3);

        return instance->luaNewIndex(context, property, value);
    } if (lua_isvector(context, 3)) {
        auto luaVector = lua_tovector(context , 3);
        Vector2 vector = {luaVector[0], luaVector[1]};

        return instance->luaNewIndex(context, property, vector);
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