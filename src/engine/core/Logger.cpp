#include "Logger.h"
#include "instances/services/ConsoleService.h"
#include <print>

using namespace Nyanners::Services;

namespace Nyanners::Core {
	void Logger::log(const std::string_view message, const std::source_location logLocation)
	{
		// CLion shows this as an error, I don't know why because it links and runs just fine.
		const std::filesystem::path full_path(logLocation.file_name());
		const std::string output = std::format("[{}:{}] {}", full_path.filename().string(), logLocation.line(), message);

		log_internal(LogLevel::Info, output);
	}

	void Logger::log_warning(const std::string_view message, const std::source_location logLocation) {
		const std::filesystem::path full_path(logLocation.file_name());
		const std::string output = std::format("[{}:{}] {}", full_path.filename().string(), logLocation.line(), message);

		log_internal(LogLevel::Warning, output);
	}

	void Logger::log_error(const std::string_view message, const std::source_location logLocation) {
		const std::filesystem::path full_path(logLocation.file_name());
		const std::string output = std::format("[{}:{}] {}", full_path.filename().string(), logLocation.line(), message);

		log_internal(LogLevel::Error, output);
	}

	std::string level_to_string(const LogLevel level) {
		switch (level) {
			case LogLevel::Info:
				return "Info";
			case LogLevel::Warning:
				return "Warning";
			case LogLevel::Error:
				return "Error";
		}
		return "Unknown";
	};

	void Logger::log_no_format(LogLevel level, const std::string_view message) {
		log_internal(level, message.data());
	}

	void Logger::log_internal(const LogLevel level, const std::string& message) {
		const auto levelString = level_to_string(level);
		const std::string output = std::format("[{}] {}", levelString, message);
		ConsoleService::log(level, output);

		if (level == LogLevel::Error) {
			std::println(stderr, "{}", output);
		} else {
			// eurgh
			std::println("{}", output);
		}
	}
}
