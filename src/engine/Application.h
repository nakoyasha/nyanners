#pragma once

#include "instances/DataModel.h"
#include "instances/datatypes/Vector.h"
#include "instances/services/io/IOService.h"
#include <memory>

#include "instances/services/RenderingService.h"
#include "instances/services/user/InputService.h"

namespace Nyanners {
  class Application {
  public:
    // std::shared_ptr<Services::RenderingService> renderService;
    // std::shared_ptr<Services::ReflectionService> reflectionService;
    // std::shared_ptr<Services::SoundService> soundService;
    // std::shared_ptr<Services::InputService> inputService;
    // std::shared_ptr<Services::RunService> runService;
    // std::shared_ptr<Services::EngineService> engineService;
    std::shared_ptr<Instances::DataModel> currentModel;

    Instances::Signal<std::shared_ptr<Instances::DataModel>> onDataModelSwitch;

    static Application* instance() {
      return m_Instance;
    }

  	Application();
    virtual ~Application();

    virtual void start();
    virtual void shutdown();
    virtual void set_datamodel(const std::shared_ptr<Instances::DataModel> &model);

    void init_rendering(const DataTypes::Vector2 &size, const std::string &windowTitle);
    bool is_rendering_enabled() const;

    static std::shared_ptr<Instances::DataModel> make_datamodel();
  protected:
    bool has_rendering = false;
  	virtual void on_update() {};
  	virtual void on_draw() const {};

    bool running = false;
    static inline Application* m_Instance;
  };
}

