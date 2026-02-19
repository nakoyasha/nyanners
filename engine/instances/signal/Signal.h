#pragma once

#include "core/Logger.h"
#include "instances/Instance.h"
#include "lua/luaImport.h"
#include "lua/reflection/Reflection.h"
#include "lua/system.h"
#include "luacode.h"
#include "luaconf.h"
#include "lualib.h"

#include <functional>

struct LuaScriptConnection {
    lua_State* context;
    int referenceId;
};

namespace Nyanners::Instances {
    template<typename... Args>
    class Signal : public Instance {
        public:
        using ConnectionCallback = std::function<void(Args...)>;

        std::vector<ConnectionCallback> connections;
        std::vector<LuaScriptConnection> luaConnections;

        Signal() : Instance("Signal") {};
        ~Signal() override
        {
            connections.clear();
        }

        void connect(ConnectionCallback callback)
        {
            connections.push_back(callback);
        }

        int connectLua(lua_State* context)
        {
            luaL_checktype(context, -1, LUA_TFUNCTION);
            lua_Debug debugInfo;
            lua_getinfo(context, 1, "S", &debugInfo);

            if (debugInfo.name == "<eval>") {
                Logger::log("Disregarding Luau connection because it's running in eval (this will result in a crash!)");
                return 0;
            }

            lua_pushvalue(context, 2);
            int refId = lua_ref(context, -1);
            luaConnections.push_back({context, refId});

            // lua_pushnumber(context ,refId);
            return 0;
        }

        int luaIndex(lua_State* context, std::string keyName) override
        {
            if (keyName == "Connect") {
                reflection_luaPushMethod(context, [this](lua_State* context) {
                    return connectLua(context);
                });
                return 1;
            } else {
                return Instance::luaIndex(context, keyName);
            }
        };

        void fire(Args... args)
        {
            for (auto connection : connections) {
                if (connection == nullptr) {
                    continue;
                }
                connection(args...);
            }

            for (auto luaConnection : luaConnections) {
                int refId = luaConnection.referenceId;

                // Logger::log(std::format("{} function id", refId));

                // get the lua function
                if (refId == LUA_REFNIL || refId == LUA_NOREF) {
                    continue;
                }

                if (!lua_checkstack(luaConnection.context, 1)) {
                    lua_throwError(luaConnection.context, "c stack overflow");
                    continue;
                }

                lua_getref(luaConnection.context, refId);

                if (auto result = lua_pcall(luaConnection.context, 0, 0, 0)) {
                    if (result != LUA_OK) {
                        const char* errorMessage = lua_tostring(luaConnection.context, -1);

                        if (errorMessage != nullptr) {
                            Logger::log(std::format("got result {} while calling signal", errorMessage));
                        } else {
                            Logger::log(std::format("unknown lua exception while processing signal {}", this->m_name));
                        }

                    }

                    lua_pop(luaConnection.context, -1);
                }
            }
        }
    };
}