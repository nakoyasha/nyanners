#pragma once
#include "lua.h"
#include "instances/Instance.h"

namespace Nyanners::Scripting {
	class LibDatatype {
	public:
		static void attach(lua_State* context);
	};
}