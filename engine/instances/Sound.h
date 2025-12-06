#pragma once
#include "instances/Instance.h"
#include "services/AssetService.h"

namespace Nyanners::Instances {

    class SoundInstance : public Instance {
    private:
        Sound audio;
        double m_volume = 1;
        bool isPlaying = false;
    public:
        std::string m_audioPath;
        bool m_looped = false;

        void setAudio(const std::string path);
        void setVolume(float newVolume);
        void play();
        void stop();
        void pause();
        void resume();

        int luaIndex(lua_State* context, std::string keyName) override;
        int luaNewIndex(lua_State* context, std::string keyName, std::string keyValue) override;
        int luaNewIndex(lua_State* context, std::string keyName, float keyValue) override;

        void update() override;
        SoundInstance() : Instance("Sound") {};
        virtual ~SoundInstance() override;
    };

} // Instances
