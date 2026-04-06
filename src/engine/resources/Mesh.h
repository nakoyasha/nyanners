#pragma once
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "instances/datatypes/Vertices.h"
#include <filesystem>

namespace Nyanners::Resources {
	class Mesh {
	public:
		int vertexCount = 0;
		int indexCount = 0;
		IndexBuffer* indexBuffer;
		VertexBuffer* vertexBuffer;

		virtual void load_from_obj_file(const std::filesystem::path &path);
		virtual void set_vertices(const DataTypes::Vertices& newVertices) = 0;
		virtual void set_indexes(const std::vector<unsigned int>& indexes) = 0;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		static Mesh* create();
		virtual ~Mesh() = default;
	};
}