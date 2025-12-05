#pragma once

#include "instances/Instance.h"
#include <functional>

namespace Nyanners::Instances {
    template<typename... Args>
    class Signal : public Instance {
        public:
        using ConnectionCallback = std::function<void(Args...)>;

        std::vector<ConnectionCallback> connections;
        Signal() : Instance("Signal") {};
        ~Signal() override
        {
            connections.clear();
        }

        void connect(ConnectionCallback callback)
        {
            connections.push_back(callback);
        }

        void fire(Args... args)
        {
            for (auto connection : connections) {
                connection(args...);
            }
        }
    };
}