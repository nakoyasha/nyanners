#pragma once
#include "Vector.h"

namespace Nyanners::DataTypes {
	class UDim2 {
	public:
		float x = 0.0f;
		float y = 0.0f;

		float absoluteX = 0.0f;
		float absoluteY = 0.0f;

		UDim2(const float x, const float y);
		UDim2();

		DataTypes::Vector2 getVector() const;
		void setX(float newX);
		void setY(float newY);
		void recomputeSize();
	};
}
