#include "Script.h"

#include "Application.h"
#include "Luau/Compiler.h"
#include "core/Logger.h"
#include "lua.h"
#include "services/ReflectionService.h"

#include <format>
#include <ranges>

#include "lualib.h"
#include "services/IOService.h"

using namespace Nyanners::Instances;


void Script::run_script() const
{
    const char* mutableGlobals[] = {"game", "DataModel", "script", nullptr};

    // Compile Luau source to bytecode
    Luau::CompileOptions compileOptions;
    compileOptions.optimizationLevel = 2;
    compileOptions.debugLevel = 2;
    compileOptions.mutableGlobals = mutableGlobals;

    const std::string bytecode = Luau::compile(source, compileOptions);

    // Load the bytecode into the Lua state
    int result = luau_load(context, name.c_str(), bytecode.data(),
                           bytecode.size(), 0);

    // push game here, to keep the reference fresh
    // reflection_exposeInstanceToLua(context, Application::instance().dataModel);
    // lua_setglobal(context, "game");

    auto model = Application::instance()->currentModel;

    Services::ReflectionService::create_reflection({
        .className = "IOService",
        .isService = true,
        .constructor = []()
        {
            return std::make_shared<Services::IOService>();
        },
        .properties = {
            {
                .name = "read_file",
                .type = ReflectionPropertyType::Method,
                .get = [](const Instance* instance, lua_State* context)
                {
                    lua_pushcfunction(context, [](lua_State* context)
                    {
                        const std::string path = luaL_checkstring(context, -1);

                        try
                        {
                            const std::string result = Services::IOService::read_file(path);
                            lua_pushstring(context, result.c_str());
                            return 1;
                        }
                        catch (std::runtime_error& e)
                        {
                            Core::Logger::log(e.what());
                            luaL_error(context, e.what());
                        }

                        return 1;
                    }, "read_file");
                    return 1;
                },
            }
        }
    });

    auto modelReflection = Services::ReflectionService::create_reflection({
        .className = "DataModel",
        .base = "Instance",
        .isService = true,
        .constructor = []()
        {
            throw std::runtime_error("You can't make a DataModel, as it's a singleton.");
            return nullptr;
        },
        .properties = {
            {
                .name = "get_service",
                .type = ReflectionPropertyType::Method,
                .get = [](const Instance* instance, lua_State* context)
                {
                    lua_pushcfunction(context, [](lua_State* context)
                    {
                        const std::string service = luaL_checkstring(context, -1);

                        for (const auto& descriptor : Services::ReflectionService::classes | std::views::values)
                        {
                            if (descriptor.className == service)
                            {
                                const auto newInstance = descriptor.constructor();
                                Services::ReflectionService::reflect_class(context, newInstance);

                                return 1;
                            }
                        }

                        return 1;
                    }, "get_service");
                    return 1;
                },
            }
        }
    });

    Services::ReflectionService::reflect_class(context, model);
    lua_setglobal(context, "DataModel");

    if (result != LUA_OK)
    {
        Core::Logger::log(std::format("Failed to compile script {}, luau_load returned {}", name, result));
    }

    if (result == LUA_OK)
    {
        const int runResult = lua_pcall(context, 0, LUA_MULTRET, 0);

        if (runResult != LUA_OK)
        {
            std::string error = lua_tostring(context, -1);
            Core::Logger::log(error);
            return;
        }
        else
        {
            printf("result: %d\n", result);
        }
    }
}
