#pragma once
#include "resources/Material.h"

namespace Nyanners::Resources::OpenGL {
	class OpenGLMaterial : public Material {
	public:
		OpenGLMaterial();

		void use() const override;
		void release() const override;

		void set_color(DataTypes::Color3 newColor) override;
		void set_texture(const std::shared_ptr<Texture> &newTexture) override;
		void set_texture(const std::filesystem::path& newTexturePath) override;

	};
}