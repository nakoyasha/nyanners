#pragma once
#include "instances/Instance.h"

#include <filesystem>

namespace Nyanners::Services {
  class IOService : public Instances::Instance {
    public:
    IOService() : Instance("IOService") {};

    static std::string read_file(const std::filesystem::path& path);
    static bool file_exists(const std::filesystem::path& path);
    static void write_file(const std::filesystem::path &path,const std::string &content);
  };
}
