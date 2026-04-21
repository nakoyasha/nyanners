#pragma once
#include "Drawable.h"
#include "instances/services/RunService.h"
#include "resources/Mesh.h"
#include <filesystem>

namespace Nyanners::Instances {
  class MeshPart : virtual public Instance, public Drawable {
  public:
    void draw() override;
    MeshPart();

    void update(const float deltaTime) override;
    void load_from_obj_file(const std::filesystem::path &path);
  	void set_vertices(const DataTypes::Vertices& newVertices) const;
  	void set_indexes(const std::vector<unsigned int>& indexes);
  private:
    std::shared_ptr<Services::RunService> runService;
  };
}
