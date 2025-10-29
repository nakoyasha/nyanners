#pragma once

#include "Instance.h"
#include "UIDrawable.h"
#include "iostream"
#include "Camera.h"

namespace Nyanners {
namespace Instances {
    class DataModel : public Instance {
    public:
        CameraInstance* camera = new CameraInstance;
        DataModel()
            : Instance("DataModel")
        {
            this->m_className = "DataModel";
            this->m_name = "Game";

            this->addChild(camera);
        };

        void draw();
        void update();
    };
}
}