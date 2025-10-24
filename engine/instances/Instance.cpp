#include "Instance.h"
#include <iostream>
#include <string>

using namespace Nyanners::Instances;

bool Instance::isA(std::string className)
{
    return this->m_className == className;
}

void Instance::clearChild(Instance* instance)
{
    // TODO: implement like actual children lmao
}

Instance::Instance(std::string className)
{
    std::cout << className << std::endl;
    m_className = className;
}

// void Instance::addChild(Instance* instance)
// {
// children[childrenIndex] = instance;
// childrenIndex += 1;
// }

Instance::~Instance()
{
    if (m_parent != nullptr) {
        m_parent->clearChild(this);
        m_parent = nullptr;
    }
}