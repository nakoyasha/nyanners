#pragma once
#include "instances/Instance.h"
#include "instances/basic/Signal.h"
#include "instances/datatypes/Vector.h"
#include "SFML/Window/Event.hpp"

namespace Nyanners::Core {
  struct EngineInfo {
    std::string version;
    std::string branch;
    std::string buildTime;
  };

  enum class EnginePlatform {
    Unknown = -1,
    Windows = 0,
    Linux = 1,
    Mac = 2,
    Android = 3,
  };
}

namespace Nyanners::Services {
  class EngineService : public Instances::Instance {
  public:
    explicit EngineService();

    [[noreturn]]
    static void panic(const std::string_view& panicMessage);

    static Instances::Signal<DataTypes::Vector2> onWindowResized;
    static Instances::Signal<const sf::Event*> onInternalEvent;

    std::string get_version() const {return engineInfo.version;}
    std::string get_branch() const {return engineInfo.branch;}
    std::string get_build_time() const {return engineInfo.buildTime;}

    static void handle_event(const sf::Event* event);
    static void open_url(const std::string& url);
  private:
    static Core::EngineInfo engineInfo;
    static Core::EnginePlatform platform;
  };
}