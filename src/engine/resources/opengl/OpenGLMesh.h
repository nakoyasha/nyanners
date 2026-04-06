#pragma once
#include "resources/Mesh.h"

namespace Nyanners::Resources::OpenGL {
	class OpenGLMesh : public Mesh {
	public:
		OpenGLMesh();
		~OpenGLMesh();

		void load_from_obj_file(const std::filesystem::path &path) override;
		void set_vertices(const DataTypes::Vertices &newVertices) override;
		void set_indexes(const std::vector<unsigned int>& indexes) override;

		void bind() const override;
		void unbind() const override;
	};
}