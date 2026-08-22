#include "ServiceProvider.h"

using namespace Nyanners::Services;

void ServiceProvider::add_service(const std::shared_ptr<Instances::Instance> &service) {
	services.insert({ service->get_name(), service });
}
