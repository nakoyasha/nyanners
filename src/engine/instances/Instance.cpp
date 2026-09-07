
#include "Instance.h"
#include "core/Logger.h"
#include "drawable/Drawable.h"
#include "services/EngineService.h"
#include "services/ReflectionService.h"

using namespace Nyanners::Instances;

Instance::~Instance() {}

void Instance::add_child(const Ref<Instance> &child) {
	const auto us = shared_from_this();

	if (auto parent = child->parent.lock()) {
		if (parent == us) {
			return;
		}

		if (parent != nullptr) {
			parent->remove_child(child);
		}
	}

	if (const auto drawable = std::dynamic_pointer_cast<Drawable>(child)) {
		this->renderableChildren.push_back(drawable);
	}

	this->children.push_back(child);
	child->parent = us;
}

void Instance::remove_child(const Ref<Instance> &child) {
	if (child->parent.lock() != shared_from_this()) {
		// return because wtf are we doing
		Core::Logger::log_error(
		  "Invalid removal; this child is not ours, thus we can't give it up for adoption."
		);
		return;
	}

	std::erase(this->children, child);
	child->parent.reset();
}

void Instance::update(const float deltaTime) {
	if (this->active != true) {
		return;
	}

	// force update of all children
	for (const auto &child : this->children) {
		child->update(deltaTime);
	}
}

Ref<Nyanners::Instances::Instance> Instance::clone() {
	auto descriptors = ReflectionDescriptorRegistry::instance()->descriptors;
	auto descriptor = descriptors.find(this->baseName);

	bool usingDefault = false;

	if (descriptor == descriptors.end()) {
		Core::Logger::log_debug(
		  "Terrible copy will be made because this instance LACKS A REFLECTION DESCRIPTOR!! PLEASE ADD ONE"
		);

		if (const auto defaultDescriptor = descriptors.find("Instance"); defaultDescriptor == descriptors.end()) {
			Services::EngineService::panic("Attempt to clone while... there's no reflection information at all...?");
		} else {
			descriptor = defaultDescriptor;
			usingDefault = true;
		}
	}

	if (descriptor->second.flags & static_cast<uint8_t>(ReflectionInstanceFlags::Service)) {
		throw std::runtime_error("This is a service and cannot be cloned");
	}

	if ((descriptor->second.flags & static_cast<uint8_t>(ReflectionInstanceFlags::NotCreatable))) {
		throw std::runtime_error("This instance cannot be created");
	}

	Ref<Instance> instance;

	if (usingDefault) {
		instance = std::make_shared<Instance>(this->baseName);
	} else {
		instance = std::dynamic_pointer_cast<Instance>(descriptor->second.construct());
	}

	auto *instancePtr = instance.get();

	for (const auto &property : Services::ReflectionService::get_properties(shared_from_this())) {
		ReflectionValue value {};
		property.get(this, value, nullptr);

		// for the best, it's probably a good idea to not do this
		if (property.type != ReflectionPropertyType::Instance && property.type != UserData && !property.has_flag(ReflectionPropertyFlags::ReadOnly)) {
			property.set(instancePtr, value, nullptr);
		}
	};

	return instance;
}

Ref<Object> Instance::clone_lua() {
	return this->clone();
}

int Instance::destroy_lua(lua_State *context) {
	const auto currentParent = this->parent.lock();
	const auto instance = Services::ReflectionService::get_instance_from_context(context, -1);

	if (currentParent != nullptr) {
		currentParent->remove_child(shared_from_this());
		this->parent.reset();
	}

	// free pointer
	instance->pointer.reset();
	lua_gc(context, LUA_GCCOLLECT, 0);

	return 0;
}

void Instance::fill_descendants_vector(List<Ref<Instance>> &descendants, const Ref<Instance>& nextInstance) {
	for (const auto& child : nextInstance->children) {
		descendants.push_back(child);
		fill_descendants_vector(descendants, child);
	}
}

List<Ref<Nyanners::Instances::Instance>> Instance::get_descendants() {
	List<Ref<Instance>> descendants {};
	fill_descendants_vector(descendants, shared_from_this());

	return descendants;
}
