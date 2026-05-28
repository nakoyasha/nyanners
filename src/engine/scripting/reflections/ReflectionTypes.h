#pragma once

namespace Nyanners::Scripting::Reflection {
	enum class ReflectionInstanceFlags {
		None = 0,
		Service = 1 << 0,
		NotCreatable = 1 << 1,
		Deprecated = 1 << 2,
	};

	enum class ReflectionPropertyFlags {
		None = 0,
		ReadOnly = 1 << 0,
		WriteOnly = 1 << 1,
		Accessible = 1 << 2,
	};

	enum class ReflectionMethodFlags {
		None = 0,
		// this is solely here for decoration, and knowing which methods will be actually interacting with the lua state
		CustomLuaState = 1 << 0,
	};

	enum ReflectionPropertyType
	{
		Unknown,
		Null,
		String,
		Number,
		Integer,
		Boolean,
		Instance,
		// Method,
		Vector3,
		Vector2,
		Color,
		UserData,
	};
}