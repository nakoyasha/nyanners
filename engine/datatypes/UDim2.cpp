
#include "UDim2.h"

#include "core/Application.h"
using namespace Nyanners::DataTypes;

void UDim2::recomputeSize() {
	const auto screenRes = Application::instance().screenSize;

	this->absoluteX = this->x * screenRes.width;
	this->absoluteY = this->y * screenRes.height;
}

void UDim2::setX(float newX) {
	this->x = newX;
	recomputeSize();
}

void UDim2::setY(float newY) {
	this->y = newY;
	recomputeSize();
}

UDim2::UDim2(const float x, const float y) : UDim2() {
	this->setX(x);
	this->setY(y);
}

UDim2::UDim2() {
	// Application::instance().onResize->connect([this](Vector2Int newSize) {
	// 	recomputeSize();
	// });
}


Vector2 UDim2::getVector() const {
	return Vector2({(float)absoluteX, (float)absoluteY});
}

