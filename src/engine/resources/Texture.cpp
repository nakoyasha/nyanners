#include "Texture.h"
#include "stb_image.h"
#include "core/Logger.h"
#include "third_party/sfml/src/SFML/Graphics/GLCheck.hpp"

using namespace Nyanners::Resources;

Texture::Texture() {
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	this->set_mipmap_enabled(true);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture() {
	glDeleteTextures(1, &textureId);
}

void Texture::load_from_file(const std::filesystem::path &path) {
	this->use();

	stbi_set_flip_vertically_on_load(1);
	textureBuffer = stbi_load(path.string().c_str(), &width, &height, &channelsInFile, 4);

	if (textureBuffer == nullptr) {
		throw std::runtime_error(std::format("Loading of texture {} failed, textureBuffer is empty", path.string()));
	}

	glCheck(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureBuffer));
	glCheck(glGenerateMipmap(GL_TEXTURE_2D));

	this->unuse();
}

void Texture::set_mipmap_enabled(const bool newState) {
	if (newState == true) {
		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
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