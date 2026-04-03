#include "MeshPart.h"
#include "Application.h"
#include "fast_obj.h"
#include "SFML/Graphics/Image.hpp"
#include "core/Logger.h"
#include "instances/services/RenderingService.h"
#include "third_party/sfml/extlibs/headers/glad/include/glad/gl.h"

using namespace Nyanners::Instances;

MeshPart::MeshPart() : Instance("MeshPart") {
	runService =
	  Application::instance()->currentModel->get_service<Services::RunService>(
	    "RunService"
	  );

	texture = new Resources::Texture();

	glBindVertexArray(vertexArrayID);
	currentShader.use();
	currentShader.setInt("uTexture", 0);

	glUseProgram(0);
	glBindVertexArray(0);
}

void MeshPart::update(const float deltaTime) {

};

void MeshPart::draw(const sf::RenderTarget &target) {
	glBindVertexArray(vertexArrayID);
	currentShader.use();
	glActiveTexture(GL_TEXTURE0);
	texture->use();

	if (indexCount == 0) {
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLint>(vertexCount));
	} else {
		glDrawElements(GL_TRIANGLES, static_cast<int>(indexCount), GL_UNSIGNED_INT, nullptr);
	}
}

void MeshPart::load_from_obj_file(const std::filesystem::path &path) {
	fastObjMesh *mesh = fast_obj_read(path.string().c_str());

	if (mesh == nullptr) {
		Core::Logger::log(std::format("Loading of mesh {} failed", path.string()));
		return;
	}

	glBindVertexArray(vertexArrayID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);

	std::vector<float> newVertices;
	newVertices.reserve(mesh->index_count * 3);

	unsigned int indexOffset = 0;

	for (unsigned int f = 0; f < mesh->face_count; ++f) {
		unsigned int fv = mesh->face_vertices[f];

		// Expect triangles
		if (fv != 3) {
			// handle error or triangulate
			continue;
		}

		for (unsigned int v = 0; v < 3; ++v) {
			const fastObjIndex &idx = mesh->indices[indexOffset + v];
			const float *p = &mesh->positions[idx.p * 3];

			// position
			newVertices.push_back(p[0]);
			newVertices.push_back(p[1]);
			newVertices.push_back(p[2]);

			// color
			// newVertices.push_back(1.0f);
			// newVertices.push_back(1.0f);
			// newVertices.push_back(1.0f);
			//
			// // texcoord
			if (idx.t != -1) {
				const float *t = &mesh->texcoords[idx.t * 2];
				newVertices.push_back(t[0]);
				newVertices.push_back(1.0f - t[1]); // OBJ V flip
			} else {
				newVertices.push_back(0.0f);
				newVertices.push_back(0.0f);
			}
		}

		indexOffset += fv;
	}

	this->set_vertices(newVertices);
}

void MeshPart::set_vertices(const DataTypes::Vertices &newVertices) {
	glBindVertexArray(vertexArrayID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);

	glBufferData(
		GL_ARRAY_BUFFER,
		static_cast<GLsizeiptr>(newVertices.size()  * sizeof(float)),
		newVertices.data(),
		GL_STATIC_DRAW
	);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	vertexCount = static_cast<int>(newVertices.size());
}

void MeshPart::set_indexes(const std::vector<unsigned>& indexes) {
	glBindVertexArray(vertexArrayID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);

	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		static_cast<GLsizeiptr>(indexes.size() * sizeof(unsigned int)),
		indexes.data(),
	GL_STATIC_DRAW
	);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	indexCount = static_cast<int>(indexes.size());
}