#pragma once
#include <filesystem>
#include <source_location>

enum TextureType {
	Texture2D = 0,
	Cubemap = 1,
};

/*
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
 */

enum TextureWrapMode {
	// based on GL_LINEAR
	Linear = 0,
	// based on GL_NEAREST
	Nearest = 1,
	// based on GL_CLAMP_TO_EDGE
	ClampToEdge = 2,
};

enum TextureFilterParameter {
	// based on GL_TEXTURE_WRAP_T
	TextureWrapCoordinateT = 0,
	// based on GL_TEXTURE_WRAP_S
	TextureWrapCoordinateS = 1,
	// based on GL_TEXTURE_MIN_FILTER
	MinificationFilter = 2,
	// based on GL_TEXTURE_MAG_FILTER
	MagnificationFilter = 3,
};

namespace Nyanners::Resources {
	class Texture : public std::enable_shared_from_this<Texture> {
	public:
		virtual ~Texture() = default;

		int height {};
		int width {};
		void  *textureBuffer {};
		std::string debugIdentifier = "texture";
		virtual void load_from_file(const std::filesystem::path& path) = 0;

		virtual void upload_buffer(
		  int internalFormat,
		  int externalFormat,
		  int width,
		  int height,
		  void *imageBuffer
		) = 0;

		virtual void set_mipmap_enabled(const bool newState) = 0;
		virtual void use() = 0;
		virtual void unuse() = 0;
		[[nodiscard]] virtual void* get_texture_handle() const = 0;

		virtual void set_texture_parameter(const TextureFilterParameter& parameter, const TextureWrapMode& wrapMode) = 0;

		static std::shared_ptr<Texture> create(const TextureType type);
		static std::shared_ptr<Texture> create(const TextureType type, const std::filesystem::path &path);
	protected:
		void load_file_into_buffer(const std::filesystem::path& path);
	private:
		bool useMipmaps = false;
		bool inUse = false;
		int channelsInFile = 4;
	};
}