#include "Object.h"

using namespace Nyanners::Instances;

Object::~Object() = default;

std::string Object::get_name() const {
	return this->name;
}

std::string Object::get_basename() const {
	return this->baseName;
}

bool Object::get_active() const {
	return this->active;
}

void Object::set_active(const bool newActiveState) {
	this->active = newActiveState;
}
