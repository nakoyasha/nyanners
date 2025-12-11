#pragma once

#include "instances/instance.h"

namespace Nyanners::Instances {
	class Workspace : public Instance {
	public:
		Workspace() : Instance("Workspace") {};
	};
}