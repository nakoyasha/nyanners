#include "AssetService.h"

#include "core/Logger.h"
#include "core/Application.h"
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
    } else if (type == AssetType::Audio) {
        auto audio = LoadSound(path.c_str());

        // this is to see if the buffer itself is null, if it is
        // we can't throw it into IsSoundValid as it causes a segmentation fault
        // and thus results in a further crash.
        if (audio.stream.processor == NULL) {
            Application::instance().panic(std::format("Fatal audio error attempting to load audio {}, possibly audio driver bug?", path));
            return {};
        }

        if (!IsSoundValid(audio)) {
            Logger::log(std::format("{} could not be loaded", path));
            return {path, AssetType::Audio, {}};
        }

        auto asset = LoadedAsset(path, AssetType::Audio, audio);
        assets.push_back(asset);
        return asset;
    }
    else {
        throw "Other asset types are currently not implemented";
    }
}

void AssetService::unloadAsset(LoadedAsset assetToUnload) {
    std::size_t currentIndex = 0;
    for (auto asset : assets) {
        if (asset.path == assetToUnload.path && asset.type == assetToUnload.type) {

            switch (asset.type) {
                case AssetType::NImage: {
                    Texture2D image = std::get<Texture2D>(asset.asset);
                    UnloadTexture(image);
                    break;
                }
                case AssetType::Audio: {
                    Sound sound = std::get<Sound>(asset.asset);
                    UnloadSound(sound);
                    break;
                }
                case AssetType::Text: {
                    std::string text = std::get<std::string>(asset.asset);
                    text.clear();
                }
                default: {
                    Logger::log("Cannot unload type");
                };
            }
            assets.erase(assets.begin() + currentIndex);
        } else {
            currentIndex++;
        }
    }
}
