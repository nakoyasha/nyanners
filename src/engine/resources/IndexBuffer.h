#pragma once
#include "instances/datatypes/Vertices.h"
#include <cstdint>

namespace Nyanners::Resources {
	class IndexBuffer
	{
	public:
		int indexCount = 0;

		virtual void use() = 0;
		virtual void release() = 0;
		virtual void upload_indices(const std::vector<uint32_t>& indices) = 0;

		virtual ~IndexBuffer() = default;

		static IndexBuffer* create();
	};
}