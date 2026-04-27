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
	this->material->use();
	Services::RenderingService::renderer->render_mesh(this->mesh);
	this->material->release();
}

void MeshPart::load_from_obj_file(const std::filesystem::path &path) {
	fastObjMesh *objMesh = fast_obj_read(path.string().c_str());

	if (objMesh == nullptr) {
		Core::Logger::log(std::format("Loading of mesh {} failed", path.string()));
		return;
	}

	std::vector<float> newVertices;
	newVertices.reserve(objMesh->index_count * 3);
	std::vector<unsigned int> newIndices;

	unsigned int indexOffset = 0;

	for (unsigned int faceIndex = 0; faceIndex < objMesh->face_count; ++faceIndex) {
		const unsigned int face = objMesh->face_vertices[faceIndex];

		for (unsigned int vertexIndex = 0; vertexIndex < 3; ++vertexIndex) {
			const fastObjIndex& idx = objMesh->indices[indexOffset + vertexIndex];
			const float* p = &objMesh->positions[idx.p * 3];

			// position
			newVertices.push_back(p[0]);
			newVertices.push_back(p[1]);
			newVertices.push_back(p[2]);

			// uv
			if (idx.t != -1) {
				const float* t = &objMesh->texcoords[idx.t * 2];
				newVertices.push_back(t[0]);
				newVertices.push_back(1.0f - t[1]); // OBJ V flip
			} else {
				newVertices.push_back(0.0f);
				newVertices.push_back(0.0f);
			}
		}

		indexOffset += face;
	}

	this->set_vertices(newVertices);
	this->mesh->bind();

	// TODO: abstract
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3));
	this->mesh->unbind();

	fast_obj_destroy(objMesh);
}

void MeshPart::set_vertices(const DataTypes::Vertices &newVertices) const {
	// glBindVertexArray(vertexArrayID);

	this->mesh->bind();
	this->mesh->set_vertices(newVertices);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));

	// glBindVertexArray(0);
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