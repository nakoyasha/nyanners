#pragma once
#include "instances/Instance.h"
#include "lua.h"

namespace Nyanners::Instances {
    class SignalBase : public Instance {
    public:
        SignalBase() : Instance("Signal") {};
        ~SignalBase() override = default;
        virtual int connectLua(lua_State* context) = 0;
    };
}