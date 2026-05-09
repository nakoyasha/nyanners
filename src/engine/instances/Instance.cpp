#include "Instance.h"
#include "core/Logger.h"
#include "drawable/Drawable.h"

using namespace Nyanners::Instances;
void Instance::add_child(const std::shared_ptr<Instance>& child) {
  const auto us = shared_from_this();

  if (child->parent == us) {
    return;
  }

  if (child->parent != nullptr) {
    child->parent->remove_child(child);
  }

	if (auto drawable = std::dynamic_pointer_cast<Drawable>(child)) {
		this->renderableChildren.push_back(drawable);
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

void Instance::update(const float deltaTime)
{
  if (this->active != true) {
    return;
  }

  // force update of all children
  for (const auto& child: this->children)
  {
    child->update(deltaTime);
  }
}

void Instance::set_active(const bool newActiveState) {
  this->active = newActiveState;

  // for (const auto& child : this->children) {
    // child->set_active(false);
//
}