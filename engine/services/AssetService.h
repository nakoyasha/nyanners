//
// Created by Haruka on 12/5/2025.
//
#pragma once
#include "instances/Instance.h"
#include <string>
#include <variant>

#include "raylib.h"

enum AssetType {
    // has to be prefixed because otherwise it conflicts with Raylib
    NImage,
    Audio,
    Text,
};

struct LoadedAsset {
    std::string path;
    AssetType type;
    std::variant<Texture2D, Sound, std::string> asset;
};

namespace Nyanners::Services {
    class AssetService : Instances::Instance {
    public:
        static std::vector<LoadedAsset> assets;
        static Texture2D tryLoadMissingTexture();
        static Texture2D missingTexture;
        AssetService() : Instances::Instance("AssetService") {};
        static LoadedAsset loadAsset(const std::string& path, const AssetType type);
    };
}
