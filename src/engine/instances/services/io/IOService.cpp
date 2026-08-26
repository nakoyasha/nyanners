#include "IOService.h"
#include "instances/services/ReflectionService.h"
#include <filesystem>
#include <fstream>

using namespace Nyanners::Services;

namespace Nyanners::Scripting {
	static auto ioServiceDescriptor =
	  ReflectionDescriptorRegistry::instance()->create_registrator([]() {
		  ReflectionService::create_descriptor("IOService", {"Instance"})
		    .add_method<&IOService::write_file>("write_file", Null,{})
	  		.add_method<&IOService::read_file>("read_file", Null,{})
	  		.add_method<&IOService::file_exists>("file_exists", Null,{});
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