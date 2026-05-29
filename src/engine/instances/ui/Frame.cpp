#include "Frame.h"

#include "instances/services/RenderingService.h"

using namespace Nyanners::Instances::UI;

namespace Nyanners::Scripting {
    static auto frameDescriptor = ReflectionDescriptorRegistry::instance()->create_registrator([]() {
       Services::ReflectionService::create_descriptor("Frame", {"UIElement"})
        .add_constructor<Frame>();
    });
}

Frame::Frame(): Instance("Frame") {
    material->shader->load_from_file("assets/shaders/2d/2d.vs", "assets/shaders/frag.glsl");
    material->shader->use();
    material->shader->setInt("uTexture", 0);
    material->shader->setBool("uTextureSet", false);
    material->shader->setBool("uFlipTexture", true);
    material->shader->release();

    set_color({255, 255, 255, 255});
}

void Frame::draw() {
    Services::RenderingService::renderer->set_depth_test(Core::Rendering::DepthCheckLevel::Always);
    Services::RenderingService::renderer->disable_depth_buffer();
    Services::RenderingService::renderer->set_renderer_feature(
      Core::Rendering::RendererFeature::FaceCulling, false
    );

    const auto absoluteSize = get_absolute_size();
    const auto absolutePosition = get_absolute_position();

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(absolutePosition.x, absolutePosition.y, 0.0f));
    transform = glm::translate(transform, -glm::vec3(anchorPoint, 0.0f));
    transform = glm::scale(transform, glm::vec3(absoluteSize, 0.0f));
    transform = glm::translate(transform, glm::vec3(anchorPoint, 0.0f));

    Services::RenderingService::renderer->render_quad(material, transform);

    Services::RenderingService::renderer->enable_depth_buffer();
    Services::RenderingService::renderer->set_previous_depth_test();
    Services::RenderingService::renderer->set_renderer_feature(Core::Rendering::RendererFeature::FaceCulling, true);

    LayerCollector::draw();
}
