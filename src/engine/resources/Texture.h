#pragma once
#include <filesystem>

enum TextureType {
	Texture2D = 0,
	Cubemap = 1,
};

namespace Nyanners::Resources {
	class Texture {
	public:
		virtual ~Texture() = default;

		int height;
		int width;
		void  *textureBuffer;

		virtual void load_from_file(const std::filesystem::path& path) = 0;

		virtual void upload_buffer(
		  int internalFormat,
		  int externalFormat,
		  int width,
		  int height,
		  const void *imageBuffer
		) const = 0;

		virtual void set_mipmap_enabled(const bool newState) = 0;
		virtual void use() = 0;
		virtual void* get_texture_handle() const = 0;
		virtual void unuse() = 0;

		static Texture* create(const TextureType type);
		static Texture* create(const TextureType type, const std::filesystem::path& path);
	protected:
		void load_file_into_buffer(const std::filesystem::path& path);
	private:
		bool useMipmaps = false;
		bool inUse = false;
		int channelsInFile = 4;
	};
}