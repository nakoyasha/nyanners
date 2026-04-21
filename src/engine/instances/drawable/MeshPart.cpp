#include "MeshPart.h"
#include "Application.h"
#include "fast_obj.h"
#include "SFML/Graphics/Image.hpp"
#include "core/Logger.h"
#include "instances/services/RenderingService.h"
#include "glad/glad.h"

using namespace Nyanners::Instances;

MeshPart::MeshPart() : Instance("MeshPart"){
	runService =
	  Application::instance()->currentModel->get_service<Services::RunService>(
	    "RunService"
	  );

	mesh = Resources::Mesh::create();

	glBindVertexArray(vertexArrayID);
	glUseProgram(0);
	glBindVertexArray(0);
}

void MeshPart::update(const float deltaTime) {

};

void MeshPart::draw() {
	glBindVertexArray(vertexArrayID);
	this->material->use();
	Services::RenderingService::renderer->render_mesh(this->mesh);
	this->material->release();
	glBindVertexArray(0);
}

void MeshPart::load_from_obj_file(const std::filesystem::path &path) {
	fastObjMesh *mesh = fast_obj_read(path.string().c_str());

	if (mesh == nullptr) {
		Core::Logger::log(std::format("Loading of mesh {} failed", path.string()));
		return;
	}

	glBindVertexArray(vertexArrayID);

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

void MeshPart::set_vertices(const DataTypes::Vertices &newVertices) const {
	glBindVertexArray(vertexArrayID);

	this->mesh->set_vertices(newVertices);
	this->mesh->bind();

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));

	glBindVertexArray(0);
	this->mesh->unbind();
}

void MeshPart::set_indexes(const std::vector<unsigned>& indexes) {
	glBindVertexArray(vertexArrayID);
	this->mesh->vertexBuffer->use();
	this->mesh->set_indexes(indexes);
	indexCount = this->mesh->indexBuffer->indexCount;
	glBindVertexArray(0);
	this->mesh->vertexBuffer->release();
}