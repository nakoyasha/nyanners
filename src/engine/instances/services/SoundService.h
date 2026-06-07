#pragma once
#include "miniaudio.h"
#include "instances/Instance.h"

namespace Nyanners::Services {
    class SoundService : public Instances::Instance {
    public:
        double volume = 1.0f;

        SoundService();
        void play_sound(const std::string& path);
        ma_sound* create_handle(const std::string& path);
        static std::string ma_result_to_string(const ma_result& result);

        double get_volume() const {return this->volume;}
        void set_volume(const double newVolume);

        ~SoundService() override;

    private:
        ma_engine soundEngine {};
    };
}