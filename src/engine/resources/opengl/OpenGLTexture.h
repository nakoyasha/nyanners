#pragma once
#include "resources/Texture.h"
#include "glad/glad.h"

namespace Nyanners::Resources::OpenGL {
	class OpenGLTexture : public Texture {
	public:
		OpenGLTexture(const TextureType type);
		OpenGLTexture(const TextureType type, const std::filesystem::path &path);
		~OpenGLTexture();

		void load_from_file(const std::filesystem::path &path) override;
		void upload_buffer(
		  int internalFormat,
		  int externalFormat,
		  int width,
		  int height,
		  void *imageBuffer
		) override;
		void set_mipmap_enabled(const bool newState) override;
		void set_texture_parameter(const TextureFilterParameter& parameter, const TextureWrapMode& wrapMode) override;
		void use() override;
		void unuse() override;

		[[nodiscard]] void *get_texture_handle() const override;
	private:
		bool useMipmaps = false;
		GLenum textureType {};
		GLuint textureId {};
	};
}