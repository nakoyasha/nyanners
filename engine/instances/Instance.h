#pragma once

#include <string>
#include <vector>

namespace Nyanners {
namespace Instances {
    class Instance {

    public:
        std::string m_name;
        std::string m_className;

        Instance* m_parent;
        std::vector<Instance*> children;

        Instance(std::string className)
        {
        }

        bool isA(std::string className)
        {
        }

        virtual void update() {
        };

        void addChild(Instance* instance)
        {
            children.push_back(instance);
        }

        void clearChild(Instance* instance)
        {
            // TODO: implement like actual children lmao
        }
    };
}
}