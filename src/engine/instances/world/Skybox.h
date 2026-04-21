#pragma once
#include "instances/Instance.h"
#include "instances/drawable/Drawable.h"

namespace Nyanners::Instances {
	class Skybox : public Instance, public Drawable {
		public:
		Skybox();

		void draw() override;
	private:
		std::shared_ptr<Resources::Texture> skyboxTexture;
	};
}