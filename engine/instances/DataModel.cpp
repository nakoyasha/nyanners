#include "../engine.h"
#include "DataModel.h"
#include "lua/system.h"

using namespace Nyanners::Instances;

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

    // render 3d
    // BeginMode3D(camera->rCamera);
    // render the rest

    for (auto instance : objects) {
        instance->draw();
    }

    // EndMode3D();
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
    }

    return Instance::luaIndex(context, keyName);
}

void DataModel::update()
{
    for (auto instance : this->children) {
        if (instance != nullptr) {
            instance->update();
        }
    }
}