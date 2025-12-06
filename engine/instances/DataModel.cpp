#include "../engine.h"
#include "DataModel.h"
#include "Script.h"
#include "lua/reflection/Reflection.h"
#include "lua/system.h"
#include "project/ProjectLoader.h"

// AssetService is included here as it must exist.
#include "services/AssetService.h"

using namespace Nyanners::Instances;

DataModel::DataModel() : Instance("DataModel")
{
    this->m_className = "DataModel";
    this->m_name = "Game";

    auto* assetService = (Instance*)new Nyanners::Services::AssetService;
    this->addChild(assetService);
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

void DataModel::draw()
{
    uiToDraw.clear();
    objects.clear();

    // filter ui
    // todo: add a layer collector for it
    for (auto instance : this->children) {
        if (instance != nullptr) {
            if (instance->isUI()) {
                uiToDraw.push_back((UIDrawable*)instance);
            } else {
                objects.push_back(instance);
            }
        }
    }

    // render 2d ui

    for (UIDrawable* ui : uiToDraw) {
        ui->draw();
    }

    for (auto instance : objects) {
        instance->draw();
    }
}

int DataModel::luaIndex(lua_State* context, std::string keyName)
{
    if (keyName == "setFPS") {
        reflection_luaPushMethod(context, [this](lua_State* context) {
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
                HttpService* existingHttp = (HttpService*)this->getChildByClass("HttpService");

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

            return 1;
        });
        return 1;
    } else if (keyName == "Tick") {
        reflection_exposeInstanceToLua(context, this->engineUpdate);
        return 1;
    } else if (keyName == "SetWindowTitle") {
        reflection_luaPushMethod(context, [this](lua_State* context) {
            luaL_checktype(context, -1, LUA_TSTRING);
            std::string newTitle = luaL_checkstring(context, -1);

            SetWindowTitle(newTitle.c_str());
            return 0;
        });
        return 1;
    }

    return Instance::luaIndex(context, keyName);
}

void DataModel::update()
{
    // for (auto* instance : children) {
    //     std::cout << "ptr: " << instance << "\n";
    // }

    for (auto instance : this->children) {
        if (instance != nullptr) {
            instance->update();
        }
    }

    // TODO: add an actual value
    this->engineUpdate->fire(1);
}