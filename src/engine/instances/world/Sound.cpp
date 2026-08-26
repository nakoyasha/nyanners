#include "Sound.h"

#include "instances/services/SoundService.h"

using namespace Nyanners::Instances;

namespace Nyanners::Scripting {
	[[maybe_unused]]
	static auto soundDescriptor =
	  ReflectionDescriptorRegistry::instance()->create_registrator([]() {
		  Services::ReflectionService::create_descriptor("Sound", {"Instance"})
		    .add_property_chained<
		      Sound,
		      double,
		      &Sound::get_volume,
		      &Sound::set_volume>("Volume", Number)
		    .add_property_chained<
		      Sound,
		      std::string,
		      &Sound::get_sound,
		      &Sound::set_sound>("SoundFile", String)
		    .add_method_anon(
		      "play",
		      [](Instances::Object *instance, lua_State *) -> int {
			      static_cast<Sound *>(instance)->play();
			      return 0;
		      },
		      Null
		    )
		    .add_method_anon(
		      "stop",
		      [](Instances::Object *instance, lua_State *) -> int {
			      static_cast<Sound *>(instance)->stop();
			      return 0;
		      },
		      Null
		    )
		    .add_constructor<Sound>();
	  });
}

void Sound::set_sound(const std::string &file) {
	if (!Services::IOService::instance()->file_exists(file)) {
		Core::Logger::log_error(
		  std::format("Cannot load sound {}: Invalid sound file", file)
		);
		return;
	}

	soundInternal = Services::SoundService::instance()->create_handle(file);

	if (soundInternal == nullptr) {
		Core::Logger::log_error(
		  std::format("Cannot load sound {}: MA Handle creation Error", file)
		);
	}
}

void Sound::set_volume(const double newVolume) {
	this->volume = newVolume;
	ma_sound_set_volume(soundInternal, newVolume);
}

void Sound::play() const {
	ma_sound_seek_to_pcm_frame(soundInternal, 0);
	ma_sound_start(soundInternal);
}

void Sound::stop() const {
	ma_sound_stop(soundInternal);
}
