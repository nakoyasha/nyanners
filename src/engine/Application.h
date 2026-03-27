#pragma once

#include "instances/DataModel.h"
#include "instances/services/EngineService.h"
#include "instances/services/IOService.h"
#include <memory>
#include <utility>

#include "instances/services/ReflectionService.h"

namespace Nyanners {
  class Application {
  public:
    std::shared_ptr<Instances::DataModel> currentModel;

    virtual ~Application();

    static Application* instance() {
      if (m_Instance == nullptr) {
        m_Instance = new Application();
      }

      return m_Instance;
    }

    Application() {
      auto model = std::make_shared<Instances::DataModel>();
      model->add_child(std::make_shared<Services::EngineService>());
      model->add_child(std::make_shared<Services::IOService>());
      model->add_child(std::make_shared<Services::ReflectionService>());

      Services::ReflectionService::register_reflections();

      this->currentModel = std::move(model);
    };

    virtual void start() const;
    virtual void shutdown();
  private:
    // const std::string projectName;
    static inline Application* m_Instance;
  };
}

