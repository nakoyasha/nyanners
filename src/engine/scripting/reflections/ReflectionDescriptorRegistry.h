#pragma once
#include "ReflectionDescriptor.h"
#include <functional>
#include <memory>
#include <string>
#include <map>

namespace Nyanners::Scripting::Reflection {
	using ReflectionRegistrator = std::function<void()>;

	class ReflectionDescriptorRegistry
	{
	public:
		std::map<std::string, ReflectionDescriptor> descriptors;
		std::vector<ReflectionRegistrator> registrators;

		static std::shared_ptr<ReflectionDescriptorRegistry> sInstance;
		static std::shared_ptr<ReflectionDescriptorRegistry> instance();

		ReflectionRegistrator create_registrator(const ReflectionRegistrator &registrator);
		void flush_registrators();
	};
}