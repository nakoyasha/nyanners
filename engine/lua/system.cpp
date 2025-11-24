#include "system.h"

using Method = void (DataModel::*)(int);

void lua_throwError(lua_State* context, std::string error)
{
    lua_pushstring(context, error.c_str());
    lua_error(context);
}

void luabridge_defineBridgeMethod(lua_State* context, std::string name,
    lua_CFunction method)
{
    lua_pushcfunction(context, method, name.c_str());
    lua_setglobal(context, name.c_str());
};

int luabridge_receiveMessageFromLua(lua_State* context)
{
    int argumentCount = lua_gettop(context);
    lua_getglobal(context, SCRIPT_NAME_GLOBAL);
    std::string scriptPath = lua_tostring(context, argumentCount + 1);
    std::string output = "";

    for (int index = 1; index <= argumentCount; index++) {
        // if (lua_isstring(context, index)) {
        size_t length = 0;
        auto argument = luaL_tolstring(context, index, &length);

        if (argument == nullptr) {
            argument = "<error during __tostring>";
        }

        if (output != "") {
            output = output + " " + argument;
        } else {
            output = output + argument;
        }
        // } {
        //     continue;
        // }
    }
    std::cout << "[Lua::" + scriptPath + "] " << output << std::endl;

    return 0;
}

// char *lua_checkstring(lua_State *context, int stackIdx) {
//   if (lua_isstring(context, stackIdx)) {
//     lua_throwError(context, "did not receive a string");
//     return nullptr;
//   }

//   const char *string = lua_tostring(context, stackIdx);
// }

int engine_LuaDrawText(lua_State* context)
{
    std::string text = luaL_checkstring(context, 1);
    DrawText(text.c_str(), 190, 200, 20, LIGHTGRAY);
    return 0;
}

int engine_LuaEnginePanic(lua_State* context)
{
    std::string text = luaL_checkstring(context, 1);
    lua_getglobal(context, SCRIPT_NAME_GLOBAL);
    std::string scriptName = lua_tostring(context, 2);

    Application::instance().panic(std::string("Panic encountered while executing ") + scriptName + ":\n" + text);
    return 0;
}

int engine_LuaEngineExit(lua_State* context)
{
    Application::instance().stop();
    return 0;
}

std::string engine_readFile(std::string fileName)
{
    std::filesystem::path filePath = fileName;

    if (!std::filesystem::is_regular_file(filePath)) {
        Application::instance().panic("engine_readFile: Invalid path: " + fileName);
    }

    std::ifstream file(fileName);
    std::string result { std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>() };

    // file.close();
    return result;
}

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
    } else {
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

static void lua_dumpstack(lua_State* L)
{
    int top = lua_gettop(L);
    for (int i = 1; i <= top; i++) {
        printf("%d\t%s\t", i, luaL_typename(L, i));
        switch (lua_type(L, i)) {
        case LUA_TNUMBER:
            printf("%g\n", lua_tonumber(L, i));
            break;
        case LUA_TSTRING:
            printf("%s\n", lua_tostring(L, i));
            break;
        case LUA_TBOOLEAN:
            printf("%s\n", (lua_toboolean(L, i) ? "true" : "false"));
            break;
        case LUA_TNIL:
            printf("%s\n", "nil");
            break;
        default:
            printf("%p\n", lua_topointer(L, i));
            break;
        }
    }
}

int engine_LuaDispatchMessage(lua_State* context)
{
    std::string message = luaL_checkstring(context, 1);

    if (message == "Engine.Exit") {
        std::cout << "Lua requested engine exit" << std::endl;
        Application::instance().stop();
    } else {
        lua_throwError(context, "Invalid dispatch message");
    }

    return 0;
};