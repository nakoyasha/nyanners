#include "IOService.h"
#include "ReflectionService.h"
#include "lualib.h"
#include <filesystem>
#include <fstream>

using namespace Nyanners::Services;

namespace Nyanners::Scripting {
	static auto ioServiceDescriptor =
	  ReflectionDescriptorRegistry::instance()->create_registrator([]() {
		  ReflectionService::create_descriptor("IOService", {"Instance"})
		    .add_method_anon(
		      "write_file",
		      [](Instances::Object *instance, lua_State *context) -> int {
			      const std::string file = luaL_checkstring(context, -2);
			      const std::string content = luaL_checkstring(context, -1);
			      IOService::write_file(file, content);
			      return 0;
		      },
		      Null
		    );
	  });
}

std::string IOService::read_file(const std::filesystem::path &path) {
	if (!file_exists(path)) {
		throw std::runtime_error(
		  std::format("Can't read file {} because it does not exist", path.string())
		);
	}

	auto file = std::ifstream(path);

	if (!file.is_open()) {
		throw std::runtime_error(std::format("Unable to open {}", path.string()));
	}

	std::string result{
	  std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()
	};

	file.close();

	return result;
}
bool IOService::file_exists(const std::filesystem::path &path) {
	if (!std::filesystem::is_regular_file(path)) {
		return false;
	}

	return true;
}

void IOService::write_file(
  const std::filesystem::path &path, const std::string &content
) {
	// if (!file_exists(path)) {
	//   throw std::runtime_error(std::format("Can't write file {} because it does not exist", path.string()));
	// }
	std::ofstream file(path);

	if (!file.is_open()) {
		throw std::runtime_error(
		  "File could not be successfully opened (invalid path or internal I/O error)"
		);
	}

	file << content;
	file.close();
}