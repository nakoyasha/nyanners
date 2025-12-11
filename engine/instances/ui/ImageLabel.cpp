#include "ImageLabel.h"
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
    Rectangle source = {0.0f, 0.0f, (float)texture.width, (float)texture.height};

    if (!IsTextureValid(texture)) return;

    DrawTexturePro(texture, source, renderingRectangle, Vector2(), 0.0f, WHITE);
}

ImageLabel::~ImageLabel() {
    this->texture = {};
    Services::AssetService::unloadAsset(this->loadedTextureAsset);
}
