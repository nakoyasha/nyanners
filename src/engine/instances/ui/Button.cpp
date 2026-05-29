#include "Button.h"
#include "instances/services/user/InputService.h"
#include "instances/services/RenderingService.h"

using namespace Nyanners::Instances;

namespace Nyanners::Scripting {
    static auto buttonRegistrator = ReflectionDescriptorRegistry::instance()->create_registrator([]() {
        Services::ReflectionService::create_descriptor("Button", {"UIElement","Transformable"})
        .add_property_chained<Button, std::shared_ptr<SignalBase>, &Button::get_activated>("Activated", ReflectionPropertyType::Instance)
        .add_property_chained<Button, std::shared_ptr<SignalBase>, &Button::get_hold>("Held", ReflectionPropertyType::Instance)
        .add_property_chained<Button, std::shared_ptr<SignalBase>, &Button::get_release>("Released", ReflectionPropertyType::Instance)
        .add_constructor<Button>();
    });
}

Button::Button(): Instance("Button") {
    material->shader->load_from_file("assets/shaders/2d/2d.vs", "assets/shaders/frag.glsl");
    material->shader->use();
    material->shader->setInt("uTexture", 0);
    material->shader->setBool("uTextureSet", false);
    material->shader->release();

    onActivated = std::make_shared<Signal<>>();
    onHold = std::make_shared<Signal<>>();
    onRelease = std::make_shared<Signal<>>();
}

void Button::draw() {
    Services::RenderingService::renderer->set_depth_test(Core::Rendering::DepthCheckLevel::Always);
    Services::RenderingService::renderer->disable_depth_buffer();
    Services::RenderingService::renderer->set_renderer_feature(
      Core::Rendering::RendererFeature::FaceCulling, false
    );

    const auto size = get_absolute_size();
    const auto absolutePosition = get_absolute_position();

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(absolutePosition.x, absolutePosition.y, 0.0f));
    transform = glm::translate(transform, -glm::vec3(anchorPoint, 0.0f));
    // transform = glm::scale(transform, glm::vec3(scale->x, scale->y, 1.0f));
    transform = glm::scale(transform, glm::vec3(size, 0.0f));
    transform = glm::translate(transform, glm::vec3(anchorPoint, 0.0f));

    const auto position2 = glm::vec2(transform[3][0], transform[3][1]);

    if (Services::InputService::is_mouse_over_button(position2, size)) {
        if (Services::InputService::is_key_down(Input::KeyCode::Mouse0)) {
            if (isPressed == false) {
                isPressed = true;
                onHold->fire();
                onActivated->fire();
                Core::Logger::log("Yayyyy");
            }
        } else {
            onRelease->fire();
            isPressed = false;
        }
    }

    Services::RenderingService::renderer->render_quad(material, transform);

    Services::RenderingService::renderer->enable_depth_buffer();
    Services::RenderingService::renderer->set_previous_depth_test();
    Services::RenderingService::renderer->set_renderer_feature(Core::Rendering::RendererFeature::FaceCulling, true);
}

std::shared_ptr<SignalBase> Button::get_activated() const {
    return this->onActivated;
}

std::shared_ptr<SignalBase> Button::get_hold() const {
    return this->onHold;
}

std::shared_ptr<SignalBase> Button::get_release() const {
    return this->onRelease;
}
