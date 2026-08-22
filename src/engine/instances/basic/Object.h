#pragma once
#include <memory>
#include <string>

namespace Nyanners::Instances {
	class Object {
	public:
		std::string name {};
		const std::string baseName {};
		bool active = true;

		explicit Object(std::string objectName) :
		  name(objectName), baseName(std::move(objectName)) {};
		virtual ~Object();

		[[nodiscard]] std::string get_name() const;
		[[nodiscard]] std::string get_basename() const;

		void set_name(const std::string &newName) {
			this->name = newName;
		}

		virtual void update(const float deltaTime) {};

		[[nodiscard]] bool get_active() const;
		virtual void set_active(bool newActiveState);
	};
}
