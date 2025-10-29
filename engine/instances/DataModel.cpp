#include "DataModel.h"

using namespace Nyanners::Instances;

void DataModel::draw() {
    std::vector<UIDrawable*> uiToDraw;
    std::vector<Instance*> objects;

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
        BeginMode3D(camera->rCamera);
        // render the rest

        for (auto instance : this->children) {
            instance->draw();
        } 
            
        EndMode3D();
}

void DataModel::update() {
    for (auto instance : this->children) {
        if (instance != nullptr) {
            instance->update();
        }
    }
}