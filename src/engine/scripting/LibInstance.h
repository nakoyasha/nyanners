#pragma once
#include "lua.h"
#include "instances/Instance.h"

namespace Nyanners::Scripting {
	class LibInstance {
	public:
		static void attach(lua_State* context);
		static std::shared_ptr<Instances::Instance> create_instance(const std::string& className);
	};
}