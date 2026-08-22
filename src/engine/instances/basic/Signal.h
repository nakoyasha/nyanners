#pragma once

#include "SignalBase.h"
#include "luaconf.h"
#include "lualib.h"
#include "core/Logger.h"
#include "instances/Instance.h"
#include <functional>
#include <format>

#include "scripting/reflections/ReflectionValueIO.h"

struct LuaScriptConnection {
    lua_State* context;
    int referenceId;
};

namespace Nyanners::Instances {
    template<typename... Args>
    class Signal : public SignalBase {
        public:
        using ConnectionCallback = std::function<void(Args...)>;

        std::vector<ConnectionCallback> connections;
        std::vector<LuaScriptConnection> luaConnections;

        Signal() = default;
        ~Signal() override
        {
            connections.clear();
        }

        void connect(ConnectionCallback callback)
        {
            connections.push_back(callback);
        }

        int connectLua(lua_State* context) override
        {
            luaL_checktype(context, -1, LUA_TFUNCTION);
            lua_Debug debugInfo;
            lua_getinfo(context, 1, "S", &debugInfo);

            if (debugInfo.name == "<eval>") {
                Core::Logger::log("Disregarding Luau connection because it's running in eval (this will result in a crash!)");
                return 0;
            }

            lua_pushvalue(context, 2);
            int refId = lua_ref(context, -1);
            luaConnections.push_back({context, refId});

            return 0;
        }

        void fire(Args... args)
        {
            auto tuple = std::make_tuple(args...);

            for (auto connection : connections) {
                if (connection == nullptr) {
                    continue;
                }
                connection(args...);
            }

            for (auto luaConnection : luaConnections) {
                int refId = luaConnection.referenceId;
                // get the lua function
                if (refId == LUA_REFNIL || refId == LUA_NOREF) {
                    continue;
                }

                if (!lua_checkstack(luaConnection.context, 1)) {
                    luaL_error(luaConnection.context, "c stack overflow");
                    continue;
                }

                lua_getref(luaConnection.context, refId);

                // push arguments
                std::apply([luaConnection](const auto&... args) {
                    ([&](const auto& item) {
                        Nyanners::Scripting::Reflection::push_value(luaConnection.context, item);
                    }(args), ...);
                }, tuple);

                if (auto result = lua_pcall(luaConnection.context, std::tuple_size_v<decltype(tuple)>, 0, 0)) {
                    if (result != LUA_OK) {
                        const char* errorMessage = lua_tostring(luaConnection.context, -1);

                        if (errorMessage != nullptr) {
                            Core::Logger::log_no_format(Core::Error, std::format("[Lua] {}", errorMessage));
                        } else {
                            Core::Logger::log_error(std::format("Script ran away while processing signal {}", this->name));
                        }
                    }

                    lua_pop(luaConnection.context, 1);
                }
            }
        }
    };
}