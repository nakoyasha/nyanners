#include "UDim2.h"
#include "instances/services/RenderingService.h"

using namespace Nyanners::DataTypes;

void UDim2::recomputeSize() {
	const auto screenRes = Services::RenderingService::renderer->get_window_size();

	this->absoluteX = this->x * screenRes.x;
	this->absoluteY = this->y * screenRes.y;
}

void UDim2::setX(const float newX) {
	this->x = newX;
	recomputeSize();
}

void UDim2::setY(const float newY) {
	this->y = newY;
	recomputeSize();
}

UDim2::UDim2(const float x, const float y) : UDim2() {
	this->setX(x);
	this->setY(y);
}

UDim2::UDim2() {
}

Nyanners::DataTypes::Vector2 UDim2::getVector() const {
	return {static_cast<uint32_t>(absoluteX), static_cast<uint32_t>(absoluteY)};
}

