#pragma once
#include <memory>

namespace Nyanners::Services {
	template <typename T>
	class Service {
		inline static std::shared_ptr<T> serviceInstance;
	public:
		static std::shared_ptr<T> instance() {
			if (serviceInstance == nullptr) {
				serviceInstance = std::make_shared<T>();
			}

			return serviceInstance;
		};
	};
}