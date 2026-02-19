//
// Created by Haruka on 19.02.2026.
//

#include "FileService.h"

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
	// TODO: move this to the service at some point
	if (!fileExists(path)) {
		throw std::runtime_error("Invalid path, either doesn't exist or isn't a file");
	}

	std::ifstream file(path);
	std::string result { std::istreambuf_iterator<char>(file),
			std::istreambuf_iterator<char>() };

	return result;
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