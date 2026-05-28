#pragma once
#include "lua.h"
#include "SFML/System/Clock.hpp"
#include "SFML/System/Time.hpp"
#include "instances/Instance.h"
#include <map>

#define LUA_SCRIPT_REGISTRY_INDEX "current_script"

namespace Nyanners::Scripting::Scheduler {
	struct ScriptResumptionDate {
		sf::Clock timeLeft;
		sf::Time pausedFor;
	};
}

namespace Nyanners::Services {
  class ScriptService : public Instances::Instance {
  public:
  	static std::map<lua_State*, Scripting::Scheduler::ScriptResumptionDate> scheduled;
  	static std::map<lua_State*, Instance*> luaToInstance;
  	static lua_State* mainContext;

    ScriptService();
    static lua_State *make_main_context();
  	static lua_State *make_context();

  	static void take_ownership_of_state(lua_State* context, Instance* owner);

  	static void run_autorun();
    static int handle_lua_console(lua_State* context);

  	static void pause_context(lua_State *context, const sf::Time &duration);
  	void update(const float deltaTime) override;
  };
}