#pragma once

#include "instances/DataModel.h"
#include "instances/datatypes/Vector.h"
#include "instances/services/user/InputService.h"
#include <memory>

#include "core/Project.h"

namespace Nyanners {
  class Application {
  public:
    Ref<DataModel> currentModel;
    Signal<Ref<DataModel>> onDataModelSwitch;

  	Core::Project project;

  	Application();
    virtual ~Application();

  	static Application* instance() {
  		return m_Instance;
  	}

    bool is_rendering_enabled() const;
    void init_rendering(const DataTypes::Vector2 &size, const std::string &windowTitle);

  	void load_default_project();
  	void load_from_project_file(const std::filesystem::path& path);
  	void load_project(const Core::Project &newProject);
    virtual void set_datamodel(const Ref<DataModel> &model);

    virtual void start();
    virtual void shutdown();
  protected:
    bool running = false;
    bool has_rendering = false;
  	static inline Application* m_Instance;

  	virtual void on_update();
    virtual void on_draw() const;
  private:
  	Core::Project defaultProject {"none", "none", "none"};
  };
}

