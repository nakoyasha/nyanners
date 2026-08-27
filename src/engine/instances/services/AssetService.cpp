#include "AssetService.h"

#include <iostream>

#include "instances/services/io/IOService.h"

using namespace Nyanners::Services;
using namespace Nyanners::Instances;

void AssetService::set_asset_root(const std::filesystem::path &newAssetRoot) {
	this->assetRoot = newAssetRoot;
}

std::string AssetService::read_file_from_assets(const std::filesystem::path &file) const {
	const auto io = IOService::instance();
	auto path = this->assetRoot;
	path.append(file.string());

	if (!io->file_exists(path)) {
		throw std::invalid_argument("Requested asset does not exist");
	}

	return io->read_file(path);
}

bool AssetService::asset_file_exists(const std::filesystem::path &file) const {
	auto path = this->assetRoot;
	path.append(file.string());
	std::cerr << "assetRoot = [" << assetRoot.string() << "]\n";
	std::cerr << "file      = [" << file.string() << "]\n";
	std::cerr << "combined  = [" << (assetRoot / file).string() << "]\n";
	std::cerr << "absolute  = [" << std::filesystem::absolute(assetRoot / file).string() << "]\n";
	return IOService::instance()->file_exists(path);
}
