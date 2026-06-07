#pragma once

#include "instances/DataModel.h"
#include "instances/datatypes/Vector.h"
#include "instances/services/IOService.h"
#include <memory>

#include "instances/services/EngineService.h"
#include "instances/services/RenderingService.h"
#include "instances/services/RunService.h"
#include "instances/services/SoundService.h"
#include "instances/services/user/InputService.h"

namespace Nyanners {
  class Application {
  public:
    std::shared_ptr<Services::RenderingService> renderService;
    std::shared_ptr<Services::ReflectionService> reflectionService;
    std::shared_ptr<Services::SoundService> soundService;
    std::shared_ptr<Services::InputService> inputService;
    std::shared_ptr<Services::RunService> runService;
    std::shared_ptr<Services::EngineService> engineService;
    std::shared_ptr<Instances::DataModel> currentModel;

    Instances::Signal<std::shared_ptr<Instances::DataModel>> onDataModelSwitch;

    virtual ~Application();

    static Application* instance() {
      return m_Instance;
    }

  	Application(const DataTypes::Vector2 size, const std::string& windowTitle);

    virtual void start();
    virtual void shutdown();
    virtual void set_datamodel(const std::shared_ptr<Instances::DataModel> &model);

    static std::shared_ptr<Instances::DataModel> make_datamodel();
  protected:
  	virtual void on_update() {};
  	virtual void on_draw() const {};
    static inline Application* m_Instance;
  };
}

