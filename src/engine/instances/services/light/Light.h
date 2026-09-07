#pragma once
#include "instances/Instance.h"
#include "instances/datatypes/Color3.h"
#include "instances/drawable/Drawable.h"

namespace Nyanners::Instances {
	class Light : public Instance, public Drawable {
	public:
		DataTypes::Color3 color {255, 255, 255, 255};
		glm::vec3 position {};
		float range = 24;

		Light();
		~Light() override;

		DataTypes::Color3 get_color() const;
		void set_color(const DataTypes::Color3 newColor);

		glm::vec3 get_position() const;
		void set_position(const glm::vec3 position);

		void draw() override;

		double get_range() const;
		void set_range(double range);
	private:
		Ref<Resources::Material> debug_IconMaterial {};
	};
}
