#pragma once

namespace Nyanners::Scripting::Reflection {
	enum ReflectionInstanceFlags {
		Service = 0,
		Creatable = 1,
		Deprecated = 2,
	};

	enum ReflectionPropertyFlags {
		ReadOnly = 0,
		WriteOnly = 1,
		Accessible = 2,
	};
}