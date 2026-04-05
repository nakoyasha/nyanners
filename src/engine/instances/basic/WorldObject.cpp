#include "WorldObject.h"
#include "instances/services/RenderingService.h"

using namespace Nyanners::Instances;

void WorldObject::set_position(const glm::vec3 &newPosition) {
	this->position = newPosition;
	this->transform = glm::translate(glm::mat4(1.0f), newPosition);
}
