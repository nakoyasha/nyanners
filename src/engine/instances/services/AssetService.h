#pragma once
#include "core/Service.h"
#include "instances/basic/Object.h"

#include <filesystem>

namespace Nyanners::Services {
	class AssetService : public Service<AssetService>, public Instances::Object {
	public:
		AssetService() : Object("AssetService") {};

		void set_asset_root(const std::filesystem::path& newAssetRoot);
		std::string read_file_from_assets(const std::filesystem::path& file) const;
		bool asset_file_exists(const std::filesystem::path& file) const;
	private:
		std::filesystem::path assetRoot = "assets";
	};
}