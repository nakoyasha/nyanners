#pragma once

#include "Instance.h"
#include "Script.h"
#include "iostream"

namespace Nyanners {
namespace Instances {
    class DataModel : public Instance {
    public:
        DataModel()
            : Instance("DataModel")
        {
            this->m_className = "DataModel";
            this->m_name = "Game";
        };

        void update()
        {
            // for (int i = 0; i < children.size(); i++) {
            //     children[i]->update();
            // }

            for (auto instance : this->children) {
                // std::cout << instance->m_className << std::endl;

                instance->update();
            }
        }
    };
}
}