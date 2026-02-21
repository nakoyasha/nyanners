//
// Created by Haruka on 19.02.2026.
//

#include "FileService.h"

#include "core/Logger.h"

using namespace Nyanners::Services;

bool FileService::fileExists(const std::string &path) {
	std::filesystem::path filePath = path;

	if (!std::filesystem::is_regular_file(filePath)) {
		return false;
	}

	return true;
}

// bool FileService::isPathValid(const std::string& path) {
// 	std::filesystem::path filePath = path;
//
// 	if (!std::filesystem::is_)
// }

std::string FileService::readFile(const std::string &path) {
	const auto schemePos = path.find("://");
	std::string scheme;

	if (schemePos != std::string::npos) {
		scheme = path.substr(0, schemePos + 3);
		Logger::log(scheme);
	} else {
		throw std::runtime_error("no file schema found, can't read");
	}

	const std::string filePath = path.substr(schemePos + 3, path.length() - schemePos - 3);

	// direct from the file system
	if (scheme == "fs://") {
		if (!fileExists(filePath)) {
			throw std::runtime_error("Invalid path, either doesn't exist or isn't a file");
		}

		std::ifstream file(filePath);
		std::string result{
			std::istreambuf_iterator<char>(file),
			std::istreambuf_iterator<char>()
		};

		return result;
	} else if (scheme == "pkg://") {
		if (filePath == "test.txt") {
			return "hi from vfs";
		}
	}
}

void FileService::writeFile(const std::string &path, const std::string &content) {
	// TODO: move this to the service at some point
	// if (fileExists(path)) {
	std::ofstream file(path);

	if (!file.is_open()) {
		throw std::runtime_error("File could not be successfully opened (invalid path or internal I/O error)");
	}

	file << content;
	// } else {
	// throw std::runtime_error("Invalid path");
	// }
}
