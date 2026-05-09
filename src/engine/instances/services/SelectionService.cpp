#include "SelectionService.h"

using namespace Nyanners::Services;

void SelectionService::set_selection(
  const std::shared_ptr<Instances::Instance> newInstance
) {
	currentSelection = newInstance;
	on_selection_changed->fire(currentSelection);
}