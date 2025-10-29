#include "Instance.h"

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

void Instance::addChild(Instance* instance)
{
    instance->m_parent = this;
    children.push_back(instance);
}

Instance::~Instance()
{
    if (m_parent != nullptr) {
        m_parent->clearChild(this);
        m_parent = nullptr;
    }
}