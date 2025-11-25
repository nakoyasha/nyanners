#pragma once

#include "Camera.h"
#include "Instance.h"
#include "UIDrawable.h"
#include "iostream"

#ifndef NO_HTTP_SERVICE
#include "services/HttpService.h"
#endif

namespace Nyanners {
namespace Instances {
    class DataModel : public Instance {
    public:
        std::vector<UIDrawable*> uiToDraw;
        std::vector<Instance*> objects;

        DataModel()
            : Instance("DataModel")
        {
            this->m_className = "DataModel";
            this->m_name = "Game";
        };

        DataModel(const std::string projectPath);

        ~DataModel()
        {
            for (UIDrawable* child : uiToDraw) {
                delete child;
            }
            uiToDraw.clear();

            for (Instance* child : objects) {
                delete child;
            }

            objects.clear();
        }

        int luaIndex(lua_State* context, std::string keyName);
        void draw();
        void update();
    };
}
}