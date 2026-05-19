#pragma once

namespace Nyanners::Scripting::Reflection {
	enum class ReflectionInstanceFlags {
		None = 0,
		Service = 0 << 0,
		Creatable = 1 << 1,
		Deprecated = 1 << 2,
	};

	enum class ReflectionPropertyFlags {
		None = 0,
		ReadOnly = 0,
		WriteOnly = 1 << 0,
		Accessible = 1 << 1,
	};

	enum class ReflectionMethodFlags {
		None = 0,
		// this is solely here for decoration, and knowing which methods will be actually interacting with the lua state
		CustomLuaState = 0,
	};

	enum ReflectionPropertyType
	{
		Unknown,
		String,
		Number,
		Boolean,
		Instance,
		// Method,
		Vector3,
		Vector2,
		Color,
		UserData,
	};
}