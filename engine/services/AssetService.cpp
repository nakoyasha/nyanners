#include "AssetService.h"

#include "core/Logger.h"
#include "lua/system.h"
#include "assets/missing.h"

using namespace Nyanners::Services;

std::vector<LoadedAsset> AssetService::assets;
Texture2D AssetService::missingTexture;

Texture2D AssetService::tryLoadMissingTexture()
{
    Image image = {
        .data = MISSING_DATA,
        .width = MISSING_WIDTH,
        .height = MISSING_HEIGHT,
        .mipmaps = 1,
        .format = MISSING_FORMAT,
    };

    Texture2D texture = LoadTextureFromImage(image);

    if (!IsTextureValid(texture)) {
        Application::instance().panic("AssetService failed to load placeholder. uh oh.");
        return {};
    }

    // UnloadImage(image);
    return texture;
}

LoadedAsset AssetService::loadAsset(const std::string& path, const AssetType type)
{
    if (!IsTextureValid(missingTexture)) {
        missingTexture = tryLoadMissingTexture();
    }

    // find and return if the asset is already loaded into memory
    for (auto asset : assets) {
        if (asset.path == path) {
            Logger::log(std::format("reusing {}", path));
            return asset;
        }
    };

    if (type == AssetType::NImage) {
        auto image = LoadTexture(path.c_str());
        // give them a placeholder instead, to avoid crashing on textures at least.
        if (!IsTextureValid(image)) {
            Logger::log(std::format("{} could not be loaded, using placeholder texture instead", path));
            return {path, AssetType::NImage, missingTexture};
        }

        auto asset = LoadedAsset(path, AssetType::NImage, image);

        assets.push_back(asset);
        return asset;
    } else if (type == AssetType::Text) {
        auto content = engine_readFile(path);
        auto asset = LoadedAsset(path, AssetType::Text, content);

        assets.push_back(asset);
        return asset;
    }
    else {
        throw "Other asset types are currently not implemented";
    }
}
