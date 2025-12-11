#include "LayerCollector.h"
#include <algorithm>
#include <ranges>

#include "core/Logger.h"
#include "lua/system.h"

using namespace Nyanners::UI;

void LayerCollector::addChild(Instance *instance) {
	if (auto uiInstance = dynamic_cast<UIDrawable*>(instance)) {
		ui.push_back(uiInstance);
	}

	this->resortVector();
	Instance::addChild(instance);
}

void LayerCollector::resortVector() {
	Nyanners::Logger::log("LayerCollector::resortVector");
	std::ranges::sort(ui, [](UIDrawable* a, UIDrawable* b) {
		return b->zIndex < a->zIndex;
	});
}

void LayerCollector::clearChild(Instance *instance) {
	auto it = std::find_if(ui.begin(), ui.end(),
		[&](const auto& child) {
				return child == instance;
		});

	if (it != ui.end())
		ui.erase(it);
}

// not permitted
void LayerCollector::luaDestroy() {};

void LayerCollector::draw() {
	if (m_enabled != true) {
		return;
	}

	for (UIDrawable* instance : std::ranges::views::reverse(ui)) {
		instance->draw();
	}
}

LayerCollector::~LayerCollector() {
	ui.clear();
	children.clear();
}



