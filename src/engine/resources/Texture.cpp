#include "Texture.h"
#include "stb_image.h"
#include "core/Logger.h"
#include "opengl/OpenGLTexture.h"

using namespace Nyanners::Resources;

std::shared_ptr<Texture> Texture::create(const TextureType type) {
	return std::make_shared<Resources::OpenGL::OpenGLTexture>(type);
}

std::shared_ptr<Texture> Texture::create(const TextureType type, const std::filesystem::path &path) {
	return std::make_shared<Resources::OpenGL::OpenGLTexture>(type, path);
}

void Texture::load_file_into_buffer(const std::filesystem::path &path) {
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path.string().c_str(), &width, &height, &channelsInFile, 0);

	if (data == nullptr) {
		throw std::runtime_error(std::format("Loading of texture {} failed", path.string()));
	}

	textureBuffer = data;
}