#pragma once
#include "Application.h"
#include "instances/Instance.h"

namespace Nyanners::Instances {
	class Sound : public Instance {
	public:
		double volume = 1.0f;
		std::string filePath = "";

		Sound() : Instance("Sound") {
		}

		std::string get_sound() const {return this->filePath;}
		void set_sound(const std::string& file);
		double get_volume() const {return this->volume;}
		void set_volume(const double newVolume);

		void play() const;
		void stop() const;
	private:
		ma_sound* soundInternal = nullptr;
	};
}