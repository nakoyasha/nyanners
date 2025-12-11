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
        Signal<int>* engineUpdate = new Signal<int>;

        DataModel();
        DataModel(const std::string projectPath);
        template <typename Service>
        Service* getOrCreateService(Service service);

        int luaIndex(lua_State* context, std::string keyName);
        void update();
    };
}
}