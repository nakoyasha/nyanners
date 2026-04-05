#pragma once

#include "instances/DataModel.h"
#include "instances/services/IOService.h"
#include <memory>

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

    Application();

    virtual void start();
    virtual void shutdown();
  protected:
  	virtual void on_update() {};
  	virtual void on_draw() const {};
  private:
    // const std::string projectName;
    static inline Application* m_Instance;
  };
}

