#include "instances/Instance.h"

using namespace Nyanners::Instances;

namespace Nyanners::Instances {
    // general inheritance class for checking
    class Service : public Instance {
    public:
        Service(std :: string className) : Instance(className) {};

        bool isA(std::string className) { 
            return className == "Service" || Instance::isA(className); 
        };
    };
}