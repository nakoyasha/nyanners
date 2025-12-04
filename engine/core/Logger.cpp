//
// Created by Haruka on 12/4/2025.
//

#include "Logger.h"
#include <iostream>
#include <print>

namespace Nyanners {
    void Logger::log(const std::string message, const std::source_location logLocation)
    {
        // CLion shows this as an error, I don't know why because it links and runs just fine.
        std::println("[{}:{}] {}", logLocation.file_name(), logLocation.line(), message.c_str());
    }

} // Nyanners