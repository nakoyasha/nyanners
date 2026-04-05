#pragma once
#include "instances/Instance.h"
#include "instances/basic/Signal.h"
#include <memory>

namespace Nyanners::Services {
	class SelectionService : public Instances::Instance {
	public:
		Instances::Signal<std::shared_ptr<Instance>>* on_selection_changed;

		SelectionService() : Instance("SelectionService") {
			on_selection_changed = new Instances::Signal<std::shared_ptr<Instance>>();
		};
		~SelectionService() override {
			delete on_selection_changed;
		};

		std::shared_ptr<Instances::Instance> currentSelection;

		void set_selection(const std::shared_ptr<Instances::Instance> newInstance) {
			currentSelection = newInstance;
			on_selection_changed->fire(currentSelection);
		}

		template<typename T>
		void get_selection() {
			return std::dynamic_pointer_cast<T>(currentSelection);
		}
	};
}