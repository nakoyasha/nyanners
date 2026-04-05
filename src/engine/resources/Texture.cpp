#include "Texture.h"
#include "SFML/Graphics/Image.hpp"
#include "core/Logger.h"
#include "third_party/sfml/src/SFML/Graphics/GLCheck.hpp"

using namespace Nyanners::Resources;

Texture::Texture() {
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(const std::filesystem::path &path) : Texture() {
	this->load_from_file(path);
}

Texture::~Texture() {
	glDeleteTextures(1, &textureId);
}

void Texture::load_from_file(const std::filesystem::path &path) {
	this->use();

	// stbi_set_flip_vertically_on_load(1);
	// textureBuffer = stbi_load(path.string().c_str(), &width, &height, &channelsInFile, 4);

	sf::Image image;

	if (!image.loadFromFile(path)) {
		throw std::runtime_error(std::format("Loading of texture {} failed", path.string()));
	}
	image.flipVertically();

	const auto size = image.getSize();
	this->upload_buffer(
	  GL_RGBA8,
	  GL_RGBA,
	  static_cast<int>(size.x),
	  static_cast<int>(size.y),
	  image.getPixelsPtr()
	);

	this->unuse();
}

void Texture::upload_buffer(const int internalFormat, const int externalFormat, const int width, const int height, const void* imageBuffer) const {
	glCheck(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, externalFormat, GL_UNSIGNED_BYTE, imageBuffer));
}

void Texture::set_mipmap_enabled(const bool newState) {
	if (newState == true) {
		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
		glCheck(glGenerateMipmap(GL_TEXTURE_2D));
	} else {
		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	}

	useMipmaps = newState;
}

void Texture::use() {
	inUse = true;
	glCheck(glBindTexture(GL_TEXTURE_2D, textureId));
}

void Texture::unuse() {
	inUse = false;
	glCheck(glBindTexture(GL_TEXTURE_2D, 0));
}