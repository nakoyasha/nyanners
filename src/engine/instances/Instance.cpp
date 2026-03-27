#include "Instance.h"

#include "core/Logger.h"

using namespace Nyanners::Instances;
void Instance::add_child(const std::shared_ptr<Instance>& child) {
  const auto us = shared_from_this();

  if (child->parent == us) {
    return;
  }

  if (child->parent != nullptr) {
    child->parent->remove_child(child);
  }

  this->children.push_back(child);
  child->parent = us;
}

void Instance::remove_child(const std::shared_ptr<Instance> &child) {
  if (child->parent != shared_from_this()) {
    // return because wtf are we doing
    Core::Logger::log("Invalid removal; this child is not ours, thus we can't give it up for adoption.");
    return;
  }

  std::erase(this->children, child);
  child->parent = nullptr;
}
