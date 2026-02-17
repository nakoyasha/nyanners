#include "ImageLabel.h"

#include <cmath>

#include "raylib.h"

using namespace Nyanners::Instances;

ImageLabel::ImageLabel() : UIDrawable("ImageLabel")
{
    this->m_name = "ImageLabel";
    this->setTexture("assets/enanui.png");
}

void ImageLabel::setTexture(const std::string path)
{
    auto asset = Services::AssetService::loadAsset(path, AssetType::NImage);
    loadedTextureAsset = asset;
    texture = std::get<Texture2D>(asset.asset);
}

int ImageLabel::luaNewIndex(lua_State* context, std::string keyName, std::string keyValue)
{
    if (keyName == "Texture") {
        this->setTexture(keyValue);
        return 1;
    } else {
        return Instance::luaNewIndex(context, keyName, keyValue);
    }
}



void ImageLabel::draw()
{
    if (!m_visible) return;
    if (!IsTextureValid(texture)) return;
    SetTextureFilter(texture, TEXTURE_FILTER_BILINEAR);
    const float aspectRatioDest = renderingRectangle.width / renderingRectangle.height;
    const float aspectRatioTexture = texture.width / texture.height;


    Rectangle source = {0.0f, 0.0f, static_cast<float>(texture.width), static_cast<float>(texture.height)};

    if (aspectRatioTexture > aspectRatioDest) {
        const float scaledWidth = texture.height * aspectRatioDest;
        const float crop = (texture.width - scaledWidth) * 0.5f;

        float left = std::clamp(crop, 0.0f, static_cast<float>(texture.width));
        float right = std::clamp(crop + scaledWidth, 0.0f, static_cast<float>(texture.width));

        source.x = crop;
        source.width = scaledWidth;
    } else {
        const float scaledHeight = texture.width / aspectRatioDest;
        const float crop = (texture.height - scaledHeight) * 0.5f;

        source.y = crop;
        source.height = scaledHeight;
    }

    DrawTexturePro(texture, source, renderingRectangle, Vector2(), 0.0f, WHITE);
}

ImageLabel::~ImageLabel() {
    this->texture = {};
    Services::AssetService::unloadAsset(this->loadedTextureAsset);
}
