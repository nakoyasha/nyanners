#include "ReflectionDescriptor.h"

using namespace Nyanners::Scripting::Reflection;

std::optional<ReflectionProperty>
ReflectionDescriptor::get_property(const std::string &propertyName) const {
	for (auto &property : properties) {
		if (property.name == propertyName) {
			return property;
		};
	}

	for (auto &parent : parents) {
		if (const auto &property = parent->get_property(propertyName);
		    property != std::nullopt) {
			return property;
		}
	}

	return std::nullopt;
}