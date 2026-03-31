#include "MeshPart.h"
#include "Application.h"
#include "fast_obj.h"
#include "SFML/Graphics/Image.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "core/Logger.h"
#include "third_party/sfml/extlibs/headers/glad/include/glad/gl.h"

using namespace Nyanners::Instances;

MeshPart::MeshPart() :
  Instance("MeshPart"), Transformable() {

  glGenBuffers(1, &vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

  glGenVertexArrays(1, &vertexArrayID);
  glBindVertexArray(vertexArrayID);

  runService = Application::instance()->currentModel->get_service<Services::RunService>("RunService");

  glGenTextures(1, &textureId);
  glBindTexture(GL_TEXTURE_2D, textureId);

  // set the texture wrapping/filtering options (on the currently bound texture object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  sf::Image image;
  if (!image.loadFromFile("assets/textures/enanui.png")) {
    throw std::runtime_error("Failed loading texture");
  }

  const auto size = image.getSize();
  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    GL_RGBA,
    size.x,
    size.y,
    0,
    GL_RGBA,
    GL_UNSIGNED_BYTE,
    image.getPixelsPtr()
  );
  glGenerateMipmap(GL_TEXTURE_2D);

  glBindVertexArray(vertexArrayID);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
  glEnableVertexAttribArray(2);

  glBindVertexArray(0);

}
void MeshPart::draw(sf::RenderTarget &target) {
  glBindVertexArray(vertexArrayID);
  // glEnableVertexAttribArray(0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textureId);

  glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 8); // Starting from vertex 0; 3 vertices total -> 1 triangle
  glBindVertexArray(0);
}

void MeshPart::update(const float deltaTime) {
  transform = glm::mat4(1.0f);
  transform = glm::translate(transform, glm::vec3(0.5f, -0.5f, 0.0f));
  transform = glm::rotate(transform, (float)runService->get_time_since_start(), glm::vec3(0.0f, 1.0f, 0.0f));
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
      const float* t = idx.t != -1 ? &mesh->texcoords[idx.t * 2] : nullptr;

      // position
      newVertices.push_back(p[0]);
      newVertices.push_back(p[1]);
      newVertices.push_back(p[2]);

      // color (temporary)
      newVertices.push_back(1.0f);
      newVertices.push_back(1.0f);
      newVertices.push_back(1.0f);

      // uv
      if (idx.t != -1) {
        const float* t = &mesh->texcoords[idx.t * 2];
        newVertices.push_back(t[0]);
        newVertices.push_back(1.0f - t[1]); // OBJ V flip
      } else {
        newVertices.push_back(0.0f);
        newVertices.push_back(0.0f);
      }
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