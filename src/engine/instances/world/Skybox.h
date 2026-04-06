#pragma once
#include "instances/Instance.h"
#include "instances/drawable/Drawable.h"

namespace Nyanners::Instances {
	class Skybox : public Instance, public Drawable {
		public:
		Skybox();

		void draw(const sf::RenderTarget &target) override;
	private:
		Resources::Texture* skyboxTexture;
	};
}