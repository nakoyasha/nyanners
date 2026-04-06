#pragma once

#include "instances/DataModel.h"
#include "instances/datatypes/Vector.h"
#include "instances/services/IOService.h"
#include <memory>

namespace Nyanners {
  class Application {
  public:
    std::shared_ptr<Instances::DataModel> currentModel;
    virtual ~Application();

    static Application* instance() {
      return m_Instance;
    }

  	Application(const DataTypes::Vector2 size, const std::string& windowTitle);

    virtual void start();
    virtual void shutdown();
  protected:
  	virtual void on_update() {};
  	virtual void on_draw() const {};
    static inline Application* m_Instance;
  };
}

