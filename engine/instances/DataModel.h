#pragma once

#include "Instance.h"
#include "ui/UIDrawable.h"

#ifndef NO_HTTP_SERVICE
#include "services/HttpService.h"
#endif

namespace Nyanners {
namespace Instances {
    class DataModel : public Instance {
    public:
        std::vector<UIDrawable*> uiToDraw;
        std::vector<Instance*> objects;

        DataModel();
        DataModel(const std::string projectPath);

        ~DataModel()
        {
            for (UIDrawable* child : uiToDraw) {
                // delete child;
            }
            uiToDraw.clear();

            for (Instance* child : objects) {
                // delete child;
            }

            objects.clear();
        }

        int luaIndex(lua_State* context, std::string keyName);
        void draw();
        void update();
    };
}
}