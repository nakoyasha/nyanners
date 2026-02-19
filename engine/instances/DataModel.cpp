#include "../engine.h"
#include "DataModel.h"
#include "Script.h"
#include "lua/reflection/Reflection.h"
#include "lua/system.h"
#include "project/ProjectLoader.h"

// AssetService is included here as it must exist.
#include "services/AssetService.h"
#include "services/ScriptService.h"
#include "services/Workspace.h"
#include "services/FileService.h"

#include <algorithm>

#include "ui/LayerCollector.h"

using namespace Nyanners::Instances;

DataModel::DataModel() : Instance("DataModel")
{
    this->m_className = "DataModel";
    this->m_name = "Game";

    this->Instance::addChild(new Workspace);
    this->Instance::addChild(new Services::AssetService);
    this->Instance::addChild(new Services::FileService);
    this->Instance::addChild(new ScriptService);
    this->Instance::addChild(new UI::LayerCollector);

    this->properties.insert({"Tick", {
        Reflection::ReflectionPropertyType::Instance,
        &engineUpdate
    }});

    this->properties.insert({"OnDraw", {
      Reflection::ReflectionPropertyType::Instance,
      &engineDraw
    }});
    this->properties.insert({"OnDrawImmediate", {
        Reflection::ReflectionPropertyType::Instance,
    &engineDraw
    }});

    this->methods.insert({"GetFPS", [this](lua_State* context) {
        auto fps = GetFPS();
        lua_pushnumber(context, fps);

        return 1;
    }});

    this->methods.insert({"SetDebug", [this](lua_State* context) {
        bool debugOn = luaL_checkboolean(context, -1);
        Application::instance().renderDebug = debugOn;

        return 0;
    }});
}

DataModel::DataModel(const std::string projectPath)
    : DataModel()
{
    std::string project = engine_readFile(projectPath);
    nlohmann::json projectJson = nlohmann::json::parse(project);

    SerializedInstanceDescriptor root = deserializeInstance(projectJson);
    this->m_name = root.name;

    for (auto child : root.children) {
        if (child.className == "Script") {
            auto filePosition = child.properties.find("file");

            if (filePosition == child.properties.end()) {
                std::cout << "Script is missing file property, which is illegal" << std::endl;
                continue;
            }

            std::string value = std::get<std::string>(filePosition->second);
            std::cout << value << std::endl;
            Script* script = new Script();
            this->addChild(script);
            script->loadFromFile(value);
        }
    }
}

int DataModel::luaIndex(lua_State* context, std::string keyName)
{
    if (keyName == "setFPS") {
        reflection_luaPushMethod(context, [](lua_State* context) {
            int fpsCap = luaL_checknumber(context, -1);
            Application::instance().setFPS(fpsCap);
            return 0;
        });
        return 1;
    } else if (keyName == "getService") {
        reflection_luaPushMethod(context, [this](lua_State* context) {
            std::string serviceName = luaL_checkstring(context, -1);

            if (serviceName == "HttpService") {
#ifdef NO_HTTP_SERVICE
                lua_throwError(context, "HttpService is not available.");
                return 0;
#else
                auto* existingHttp = static_cast<HttpService *>(this->getChildByClass("HttpService"));

                if (existingHttp != nullptr) {
                    reflection_exposeInstanceToLua(context, existingHttp);
                } else {
                    Nyanners::Instances::HttpService* http = new Nyanners::Instances::HttpService;
                    this->addChild(http);
                    reflection_exposeInstanceToLua(context, http);
                }

                return 1;
#endif
            }
            // these are guaranteed to exist
            else if (serviceName == "Workspace") {
                reflection_exposeInstanceToLua(context, this->getChildByClass("Workspace"));
                return 1;
            } else if (serviceName == "ScriptService") {
                reflection_exposeInstanceToLua(context, this->getChildByClass("ScriptService"));
                return 1;
            } else if (serviceName == "FileService") {
                reflection_exposeInstanceToLua(context, this->getChildByClass("FileService"));
                return 1;
            }

            return 1;
        });
        return 1;
    } else if (keyName == "Tick") {
        reflection_exposeInstanceToLua(context, this->engineUpdate);
        return 1;
    } else if (keyName == "SetWindowTitle") {
        reflection_luaPushMethod(context, [](lua_State* context) {
            luaL_checktype(context, -1, LUA_TSTRING);
            std::string newTitle = luaL_checkstring(context, -1);

            SetWindowTitle(newTitle.c_str());
            return 0;
        });
        return 1;
    } else if (keyName == "SetWindowSize") {
        reflection_luaPushMethod(context, [](lua_State* context) {
            const float* luaVector = luaL_checkvector(context, -1);

            SetWindowSize(static_cast<int>(luaVector[0]), static_cast<int>(luaVector[1]));

            // https://github.com/Omegapy/MyRaylibFunctions/blob/75328a1a52101c5cac5afe2968989fd717b8a346/my_raylib_functions.hpp#L90
            int monitor = GetCurrentMonitor();// Get current connected monitor
            int monitor_width = GetMonitorWidth(monitor); // Get specified monitor width (current video mode used by monitor)
            int monitor_height = GetMonitorHeight(monitor); // Get specified monitor height (current video mode used by monitor)
            SetWindowPosition((int)(monitor_width / 2) - (int)(luaVector[0] / 2), (int)(monitor_height / 2) - (int)(luaVector[1] / 2));

            return 0;
       });
        return 1;
    }

    return Instance::luaIndex(context, keyName);
}

void DataModel::update()
{
    Instance::update();

    // TODO: add an actual value
    this->engineUpdate->fire(1);
}

void DataModel::draw() {
    Instance::draw();
    this->engineDraw->fire(1);
}