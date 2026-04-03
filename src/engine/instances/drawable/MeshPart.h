#pragma once
#include "Drawable.h"
#include "instances/Instance.h"
#include "instances/services/RunService.h"
#include "resources/Texture.h"
#include <filesystem>

namespace Nyanners::Instances {
  class MeshPart : public Instance, public Drawable {
  public:
  	Resources::Texture* texture;

    void draw(const sf::RenderTarget &target) override;
    MeshPart();
  	~MeshPart() = default;

    void update(const float deltaTime) override;
    void load_from_obj_file(const std::filesystem::path &path);
  	void set_vertices(const DataTypes::Vertices& newVertices);
  	void set_indexes(const std::vector<unsigned int>& indexes);
  private:
    std::shared_ptr<Services::RunService> runService;
  };
}
