#pragma once
#include "instances/datatypes/Vertices.h"
#include <cstdint>

namespace Nyanners::Resources {
	class VertexBuffer
	{
	public:
		int vertexCount = 0;

		virtual void use() = 0;
		virtual void release() = 0;
		virtual void upload_vertices(const DataTypes::Vertices& vertices) = 0;

		virtual ~VertexBuffer() = default;

		static VertexBuffer* create();
	};
}