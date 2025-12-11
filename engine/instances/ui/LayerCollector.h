#pragma once
#include "instances/Instance.h"
#include "instances/ui/UIDrawable.h"

using namespace Nyanners::Instances;

namespace Nyanners::UI {
		class LayerCollector : public Instance {
		private:
			std::vector<UIDrawable*> ui;
		public:
			bool m_enabled = true;
			LayerCollector() : Instance("LayerCollector") {
				this->properties.insert({"Enabled", {
						Reflection::ReflectionPropertyType::Bool,
			&m_enabled
				}});
			};
			~LayerCollector() override;

			void draw() override;
			void addChild(Instance* instance) override;
			void clearChild(Instance* instance) override;

			void luaDestroy() override;

			// Resorts all items by their ZIndex, to account for ZIndex updates.
			void resortVector();
		};
}