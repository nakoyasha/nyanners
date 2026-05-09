#include "Skybox.h"
#include "stb_image.h"
#include "instances/services/RenderingService.h"

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

			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);

			stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	material->set_shader("assets/shaders/skybox/vertex.glsl", "assets/shaders/skybox/frag.glsl");
	material->set_texture(skyboxTexture);

	glBindVertexArray(vertexArrayID);

	// notice of ai-generated code:
	// yeah i couldn't be bothered here either to be honest. then again
	// counting duplicate vertices isn't exactly fun so i think it's fair
	this->mesh->set_vertices({
			// front
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,

			// back
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,

			// left
			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			// right
			 1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,

			// top
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			// bottom
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f
	});

	this->mesh->set_indexes({
		 0,  1,  2,   2,  3,  0,   // front
		 4,  5,  6,   6,  7,  4,   // back
		 8,  9, 10,  10, 11,  8,   // left
		12, 13, 14,  14, 15, 12,   // right
		16, 17, 18,  18, 19, 16,   // top
		20, 21, 22,  22, 23, 20    // bottom
	});

	this->mesh->bind();
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);
	this->mesh->unbind();

	glBindVertexArray(0);
}

void Skybox::draw() {
	// glDepthFunc(GL_LEQUAL);
	Services::RenderingService::renderer->set_depth_test(Core::Rendering::LessThanAndEqual);
	Services::RenderingService::renderer->disable_depth_buffer();
	glCullFace(GL_FRONT);

	this->material->use();
	glBindVertexArray(vertexArrayID);
	Services::RenderingService::renderer->render_mesh(this->mesh);
	glBindVertexArray(0);

	// glDepthFunc(GL_LESS);
	// glDepthMask(GL_TRUE);
	glCullFace(GL_BACK);
	Services::RenderingService::renderer->set_previous_depth_test();
	Services::RenderingService::renderer->enable_depth_buffer();
}