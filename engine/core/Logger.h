#pragma once
#include <string>
#include <string_view>
#include <source_location>

namespace Nyanners {
    class Logger {
        Logger() = default;
        ~Logger() = default;

    public:
        static void log(const std::string message, const std::source_location = std::source_location::current());
    };
} // Nyanners
