#pragma once

#include "instances/DataModel.h"
#include "instances/datatypes/Vector.h"
#include "instances/services/user/InputService.h"
#include <memory>

namespace Nyanners {
  class Application : Services::Service<Application> {
  public:
    Ref<DataModel> currentModel;
    Signal<Ref<DataModel>> onDataModelSwitch;

  	Application();
    virtual ~Application();

  	static Application* instance() {
  		return m_Instance;
  	}

    bool is_rendering_enabled() const;
    void init_rendering(const DataTypes::Vector2 &size, const std::string &windowTitle);

    static Ref<DataModel> make_datamodel();
    virtual void set_datamodel(const Ref<DataModel> &model);

    virtual void start();
    virtual void shutdown();
  protected:
    bool running = false;
    bool has_rendering = false;
  	static inline Application* m_Instance;

  	virtual void on_update();

    virtual void on_draw() const;
  };
}

