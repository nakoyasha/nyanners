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

std::optional<ReflectionMethod> ReflectionDescriptor::get_method(const std::string &methodName) const {
	for (const auto &method : methods) {
		if (method.name == methodName) {
			return method;
		};
	}

	for (auto &parent : parents) {
		if (const auto &method = parent->get_method(methodName); method != std::nullopt) {
			return method;
		}
	}

	return std::nullopt;
}