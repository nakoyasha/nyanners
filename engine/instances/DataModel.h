#pragma once

#include "Instance.h"
#include "ui/UIDrawable.h"
#include "instances/signal/Signal.h"

#ifndef NO_HTTP_SERVICE
#include "services/HttpService.h"
#endif

namespace Nyanners {
namespace Instances {
    class DataModel : public Instance {
    public:
        std::vector<UIDrawable*> uiToDraw;
        std::vector<Instance*> objects;
        Signal<int>* engineUpdate = new Signal<int>;

        DataModel();
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

        template <typename Service>
        Service* getOrCreateService(Service service);

        int luaIndex(lua_State* context, std::string keyName);
        void draw();
        void update();
    };
}
}