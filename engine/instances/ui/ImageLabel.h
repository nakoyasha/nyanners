#pragma once
#include "UIDrawable.h"
#include "raylib.h"
#include "services/AssetService.h"

namespace Nyanners::Instances {
    class ImageLabel : public UIDrawable {
    public:
        ImageLabel();
        std::string texturePath;
        void draw() override;
        void setTexture(const std::string path);

        int luaNewIndex(lua_State* context, std::string keyName, std::string keyValue) override;
        virtual ~ImageLabel();
    private:
        Texture2D texture;
        LoadedAsset loadedTextureAsset;
    };
}
