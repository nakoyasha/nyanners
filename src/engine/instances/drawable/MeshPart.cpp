#include "MeshPart.h"
#include "Application.h"
#include "fast_obj.h"
#include "core/Logger.h"
#include "third_party/sfml/extlibs/headers/glad/include/glad/gl.h"

using namespace Nyanners::Instances;

MeshPart::MeshPart() :
  Instance("MeshPart"), Transformable() {
  // This will identify our vertex buffer
  // Generate 1 buffer, put the resulting identifier in vertexbuffer
  glGenBuffers(1, &vertexBuffer);
  // The following commands will talk about our 'vertexbuffer' buffer
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

  runService = Application::instance()->currentModel->get_service<Services::RunService>("RunService");

}
void MeshPart::draw(sf::RenderTarget &target) {
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glVertexAttribPointer(
     0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
     3,                  // size
     GL_FLOAT,           // type
     GL_FALSE,           // normalized?
     0,                  // stride
     (void*)0            // array buffer offset
  );

  // Draw the triangle !
  glDrawArrays(GL_TRIANGLES, 0, vertices.size()); // Starting from vertex 0; 3 vertices total -> 1 triangle
  glDisableVertexAttribArray(0);
}
void MeshPart::update(const float deltaTime) {
  transform = glm::mat4(1.0f);
  transform = glm::translate(transform, glm::vec3(0.5f, -0.5f, 0.0f));
  transform = glm::rotate(transform, (float)runService->get_time_since_start(), glm::vec3(0.0f, 0.0f, 1.0f));
}

void MeshPart::load_from_obj_file(const std::filesystem::path &path) {
  fastObjMesh* mesh = fast_obj_read(path.string().c_str());

  if (mesh == nullptr) {
    Core::Logger::log(std::format("Loading of mesh {} failed", path.string()));
    return;
  }

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
      const fastObjIndex& idx = mesh->indices[indexOffset + v];

      const float* p = &mesh->positions[idx.p * 3];
      newVertices.push_back(p[0]);
      newVertices.push_back(p[1]);
      newVertices.push_back(p[2]);
    }

    indexOffset += fv;
  }

  vertices = newVertices;

  // Give our vertices to OpenGL.
  glBufferData(
    GL_ARRAY_BUFFER,
    vertices.size() * sizeof(GLfloat),
    vertices.data(),
    GL_STATIC_DRAW
  );
}