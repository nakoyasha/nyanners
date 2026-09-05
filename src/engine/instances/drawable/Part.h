#pragma once
#include "Drawable.h"

namespace Nyanners::Instances {
	class Part : virtual public Instance, public Drawable {
	public:
		void draw() override;
		Part();
	};
}
