#include "Sound.h"

#include "lua/reflection/Reflection.h"

using namespace Nyanners::Instances;

void SoundInstance::setAudio(const std::string path)
{
    auto asset = Services::AssetService::loadAsset(path, AssetType::Audio);
    bool wasPlayingBeforeLoad = isPlaying;

    auto newloadedSoundAsset = asset;
    auto newAudio = std::get<Sound>(newloadedSoundAsset.asset);

    if (IsSoundValid(newAudio)) {

        if (wasPlayingBeforeLoad) {
            this->stop();
            // TODO: figure out a reference system or something?
            // unloading on sound change is not preferable as this results in
            // stutters between swaps
            // Services::AssetService::unloadAsset(this->loadedSoundAsset);
        }

        loadedSoundAsset = newloadedSoundAsset;
        audio = newAudio;
    }

    if (wasPlayingBeforeLoad) {
        this->play();
    }
}

void SoundInstance::setVolume(float newVolume)
{
    this->m_volume = newVolume;
    SetSoundVolume(this->audio, m_volume);
}

void SoundInstance::update()
{
    // this is a very jank way of telling raylib to play a sound in a loop
    // basically: if the sound is no longer playing but we knew it was playing before -> if it's looped, then play it again, else just set isPlaying to false
    if (!IsSoundPlaying(this->audio) && isPlaying == true) {
        if (m_looped) {
            // play again, as we're looped
            this->play();
        } else {
            isPlaying = false;
        }
    }
}

void SoundInstance::play()
{
    // can't play audio when it's not valid, otherwise the
    // big bad wolf will come around and blow the engine away
    // as it's held by sticks
    if (!IsSoundValid(this->audio)) {
        return;
    }

    // make sure it plays at the correct volume
    isPlaying = true;
    SetSoundVolume(this->audio, m_volume);
    PlaySound(this->audio);
}

void SoundInstance::stop()
{
    // set isPlaying to false here as otherwise we'd loop
    isPlaying = false;
    StopSound(this->audio);
}

void SoundInstance::pause() {
    isPlaying = false;
    PauseSound(this->audio);
}

void SoundInstance::resume() {
    isPlaying = true;
    ResumeSound(this->audio);
}

int SoundInstance::luaIndex(lua_State* context, std::string keyName)
{
    if (keyName == "Play") {
        reflection_luaPushMethod(context, [this](lua_State* context) {
            this->play();
            return 0;
        });
        return 1;
    } else if (keyName == "Stop") {
        reflection_luaPushMethod(context, [this](lua_State* context) {
            this->stop();
            return 0;
        });
        return 1;
    } else if (keyName == "Pause") {
        reflection_luaPushMethod(context, [this](lua_State* context) {
            this->pause();
            return 0;
        });
        return 1;
    } else if (keyName == "Resume") {
        reflection_luaPushMethod(context, [this](lua_State* context) {
            this->resume();
            return 0;
        });
        return 1;
    }

    return Instance::luaIndex(context, keyName);
}

int SoundInstance::luaNewIndex(lua_State* context, std::string keyName, std::string keyValue)
{
    if (keyName == "Sound") {
        this->setAudio(keyValue);
        return 0;
    }

    return Instance::luaNewIndex(context, keyName, keyValue);
}

int SoundInstance::luaNewIndex(lua_State* context, std::string keyName, float keyValue)
{
    if (keyName == "Volume") {
        this->setVolume(keyValue);
        return 0;
    }

    return Instance::luaNewIndex(context, keyName, keyValue);
}

int SoundInstance::luaNewIndex(lua_State* context, std::string keyName, bool keyValue)
{
    if (keyName == "Looped") {
        this->m_looped = keyValue;
        return 0;
    }

    return Instance::luaNewIndex(context, keyName, keyValue);
}

SoundInstance::~SoundInstance() {
    if (IsSoundValid(this->audio)) {
        if (IsSoundPlaying(this->audio)) {
            this->stop();
        }
        // TODO: unload audio through AssetService
        Services::AssetService::unloadAsset(this->loadedSoundAsset);
        // UnloadSound(this->audio);
    }
}

