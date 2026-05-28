#pragma once
#include "Instance.h"
#include "Luau/Compiler.h"
#include "services/ScriptService.h"
#include <filesystem>

namespace Nyanners::Instances {
  class Script : public Instance {
  public:
    std::string source;
    std::filesystem::path filePath;
    lua_State* context;

    Script();;

    void initialize_script();

    std::string get_file_path() const;
    void set_file(const std::string& scriptPath);
  	void reload();
    void set_source(std::string &source);
    void run_script();
    void set_active(const bool active) override;
  private:
    bool isRunning = false;
    bool wasPreviouslyRunning = false;
    std::string bytecode;
    Luau::CompileOptions compileOptions {.optimizationLevel = 2, .debugLevel = 2};
  };
}