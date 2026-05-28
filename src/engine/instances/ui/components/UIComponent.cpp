#include "UIComponent.h"

#include "instances/services/ReflectionService.h"
#include "scripting/reflections/ReflectionDescriptorRegistry.h"

using namespace Nyanners::UI::Components;

namespace Nyanners::Scripting {
    static auto uiComponent = ReflectionDescriptorRegistry::instance()->create_registrator([]() {
       Services::ReflectionService::create_descriptor("UIComponent", {});
    });
}