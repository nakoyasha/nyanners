#pragma once
#include <cstdint>

namespace Nyanners::DataTypes {
	struct Vector2 {
		uint32_t x;
		uint32_t y;
		uint32_t width;
		uint32_t height;
	};

	struct Vector3 {
		uint32_t x;
		uint32_t y;
		uint32_t z;
	};
}