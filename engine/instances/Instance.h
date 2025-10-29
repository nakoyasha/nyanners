#pragma once

#include <string>
#include <vector>
#include <iostream>

namespace Nyanners {
namespace Instances {
    class Instance {

    public:
        std::string m_name;
        std::string m_className;

        Instance* m_parent;
        std::vector<Instance*> children;

        Instance(std::string className);
        ~Instance();
        bool isA(std::string className);

        virtual void update() { };
        virtual void draw() {};

        virtual bool isUI() {
            return false;
        }

        void addChild(Instance* instance);

        // TODO: implement like actual children lmao
        void clearChild(Instance* instance);
    };
}
}