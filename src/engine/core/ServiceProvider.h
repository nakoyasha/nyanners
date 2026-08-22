#pragma once
#include <unordered_map>
#include "Service.h"
#include "instances/Instance.h"

namespace Nyanners::Services {
	class ServiceProvider : public Service<ServiceProvider> {
	public:
		std::unordered_map<std::string, std::shared_ptr<Instances::Instance>> services;

		template <typename T>
		std::shared_ptr<T> get_service(const std::string &name) {
			if (this->services.find(name) == nullptr) {
				throw std::runtime_error("Invalid service name");
			}

			return std::dynamic_pointer_cast<T>(this->services.at(name));
		}

		template <typename T>
		std::shared_ptr<T> add_service() {
			auto service = std::make_shared<T>();

			// if (services.find(service->get_name()) != nullptr) {
			// ~service();
			// throw std::runtime_error("Service already exists");
			// }

			services.insert({ service->get_name(), service });

			return std::dynamic_pointer_cast<T>(service);
		}

		void add_service(const std::shared_ptr<Instances::Instance> &service);

	private:
	};
}
