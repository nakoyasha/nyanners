#pragma once

#include "Camera.h"
#include "Instance.h"
#include "UIDrawable.h"
#include "iostream"
#include "services/HttpService.h"

namespace Nyanners {
namespace Instances {
    class DataModel : public Instance {
    public:
        CameraInstance* camera = new CameraInstance;
        std::vector<UIDrawable*> uiToDraw;
        std::vector<Instance*> objects;

        DataModel()
            : Instance("DataModel")
        {
            this->m_className = "DataModel";
            this->m_name = "Game";

            this->addChild(camera);
        };

        int luaIndex(lua_State* context, std::string keyName);
        void draw();
        void update();
    };
}
}