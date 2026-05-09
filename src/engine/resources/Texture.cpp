#include "Texture.h"
#include "stb_image.h"
#include "core/Logger.h"
#include "instances/services/RenderingService.h"
#include "opengl/OpenGLTexture.h"

using namespace Nyanners::Resources;
using namespace OpenGL;

std::shared_ptr<Texture> Texture::create(const TextureType type) {
	const auto texture = std::make_shared<OpenGLTexture>(type);
	Services::RenderingService::add_texture(texture);

	return texture;
}

std::shared_ptr<Texture> Texture::create(const TextureType type, const std::filesystem::path &path) {
	const auto texture = std::make_shared<OpenGLTexture>(type, path);
	Services::RenderingService::add_texture(texture);

	return texture;
}

void Texture::load_file_into_buffer(const std::filesystem::path &path) {
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path.string().c_str(), &width, &height, &channelsInFile, 4);

	if (data == nullptr) {
		throw std::runtime_error(std::format("Loading of texture {} failed", path.string()));
	}

	textureBuffer = data;
}