#include "ReflectionDescriptorRegistry.h"
using namespace Nyanners::Scripting::Reflection;

std::shared_ptr<ReflectionDescriptorRegistry> ReflectionDescriptorRegistry::sInstance;

std::shared_ptr<ReflectionDescriptorRegistry>
ReflectionDescriptorRegistry::instance() {
	if (sInstance == nullptr) {
		sInstance = std::make_shared<ReflectionDescriptorRegistry>();
	}

	return sInstance;
}

ReflectionRegistrator ReflectionDescriptorRegistry::create_registrator(
  const ReflectionRegistrator &registrator
) {
	registrators.push_back(registrator);
	return registrator;
}

void ReflectionDescriptorRegistry::flush_registrators() {
	for (auto &registrator : registrators) {
		registrator();
		// TODO: check for re-defined descriptors?
	}

	registrators.clear();
}