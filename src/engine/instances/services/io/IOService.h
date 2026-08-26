#pragma once
#include "instances/Instance.h"

#include <filesystem>

#include "core/Service.h"

namespace Nyanners::Services {
  class IOService : public Instances::Instance, public Service<IOService> {
    public:
    IOService() : Instance("IOService") {};

  	std::string read_file(const std::filesystem::path& path);
    bool file_exists(const std::filesystem::path& path);
  	void write_file(const std::filesystem::path &path,const std::string &content);
  };
}
