#pragma once
#include "instances/Instance.h"

namespace Nyanners::Instances {
	class Folder : public Instance {
	public:
		Folder() : Instance("Folder") {};
	};

	class Model : public Instance {
	public:
		Model() : Instance("Model") {};
	};

	void link_basic_containers();
}