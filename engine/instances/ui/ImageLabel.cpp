//
// Created by Haruka on 12/4/2025.
//

#include "ImageLabel.h"
#include "services/AssetService.h"
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
    texture = std::get<Texture2D>(asset.asset);
}

int ImageLabel::luaNewIndex(lua_State* context, std::string keyName, std::string keyValue)
{
    if (keyName == "Texture") {
        this->setTexture(keyValue);
        return 1;
    } else {
        return UIDrawable::luaNewIndex(context, keyName, keyValue);
    }
}

void ImageLabel::draw()
{
    DrawTexture(texture, position.x, position.y, WHITE);
}
