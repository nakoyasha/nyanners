#include "Skybox.h"
#include "stb_image.h"
#include "third_party/sfml/src/SFML/Graphics/GLCheck.hpp"

using namespace Nyanners::Instances;

Skybox::Skybox() : Instance("Skybox") {
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

	std::vector<std::string> files = {
		"assets/textures/skybox/txStormydays_right.png",
		"assets/textures/skybox/txStormydays_left.png",
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
	}

	glCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	glCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	glCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	glCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	glCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

	currentShader = Resources::Shader();
	currentShader.load_from_file("assets/shaders/skybox/vertex.glsl", "assets/shaders/skybox/frag.glsl");

	glCheck(glBindVertexArray(vertexArrayID));

	vertices = {
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
	};

	glCheck(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId));

	glCheck(glBufferData(
		GL_ARRAY_BUFFER,
		static_cast<GLsizeiptr>(vertices.size()  * sizeof(float)),
		vertices.data(),
		GL_STATIC_DRAW
	));

	glCheck(glEnableVertexAttribArray(0));
	glCheck(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr));

	glCheck(glBindVertexArray(0));
}

void Skybox::draw(const sf::RenderTarget& target) {
	glCheck(glDepthMask(GL_FALSE));
	glCheck(glDepthFunc(GL_LEQUAL));

	currentShader.use();

	glActiveTexture(GL_TEXTURE0);
	glCheck(glBindTexture(GL_TEXTURE_CUBE_MAP, textureId));

	glCheck(glBindVertexArray(vertexArrayID));
	glCheck(glDrawArrays(GL_TRIANGLES, 0, vertices.size()));
	glCheck(glBindVertexArray(0));

	glCheck(glDepthFunc(GL_LESS));
	glCheck(glDepthMask(GL_TRUE));
}