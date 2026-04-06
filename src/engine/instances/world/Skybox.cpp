#include "Skybox.h"
#include "stb_image.h"
#include "instances/services/RenderingService.h"
#include "third_party/sfml/src/SFML/Graphics/GLCheck.hpp"

using namespace Nyanners::Instances;

Skybox::Skybox() : Instance("Skybox") {
	skyboxTexture = Resources::Texture::create(TextureType::Cubemap);
	skyboxTexture->use();

	std::vector<std::string> files = {
		"assets/textures/skybox/txStormydays_left.png",
		"assets/textures/skybox/txStormydays_right.png",
		"assets/textures/skybox/txStormydays_up.png",
		"assets/textures/skybox/txStormydays_down.png",
		"assets/textures/skybox/txStormydays_front.png",
		"assets/textures/skybox/txStormydays_back.png",
	};

	int width, height, nrChannels;
	unsigned char *data;

	// stbi_set_flip_vertically_on_load(1);
	for (unsigned int i = 0; i < files.size(); i++) {
			auto file = files[i];

			data = stbi_load(file.c_str(), &width, &height, &nrChannels, 3);

			glCheck(glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			));

			stbi_image_free(data);
	}

	glCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	glCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	glCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	glCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	glCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

	material->set_shader("assets/shaders/skybox/vertex.glsl", "assets/shaders/skybox/frag.glsl");
	material->set_texture(skyboxTexture);

	glCheck(glBindVertexArray(vertexArrayID));

	this->mesh->set_vertices({
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	});

	this->mesh->vertexBuffer->use();
	glCheck(glEnableVertexAttribArray(0));
	glCheck(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr));
	this->mesh->vertexBuffer->release();

	glCheck(glBindVertexArray(0));
}

void Skybox::draw(const sf::RenderTarget& target) {
	glCheck(glDepthMask(GL_FALSE));
	glCheck(glDepthFunc(GL_LEQUAL));

	this->material->use();
	glCheck(glBindVertexArray(vertexArrayID));
	Services::RenderingService::render_mesh(this->mesh);
	glCheck(glBindVertexArray(0));

	glCheck(glDepthFunc(GL_LESS));
	glCheck(glDepthMask(GL_TRUE));
}