#pragma once
#include "Instance.h"
#include "Luau/Compiler.h"
#include "data/UserdataTags.h"
#include "services/ScriptService.h"
#include <filesystem>

namespace Nyanners::Instances {
  class Script : public Instance {
  public:
    std::string source;
    std::filesystem::path filePath;
    lua_State* context;

    Script() : Instance("Script") {
    };

    void initialize_script();
    void set_file(const std::filesystem::path& scriptPath);
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