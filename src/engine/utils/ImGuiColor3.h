#pragma once
#include "imgui.h"
#include "instances/datatypes/Color3.h"

namespace Nyanners::Debug {
	inline ImVec4 color3_to_imvec4(const DataTypes::Color3& color) {
		return {color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.alpha / 255.0f};
	};
}