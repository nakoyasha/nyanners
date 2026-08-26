#include "Logger.h"
#include "instances/services/ConsoleService.h"
#include <source_location>
#include <filesystem>
#include <print>

using namespace Nyanners::Services;

namespace Nyanners::Core {
	void Logger::log(const std::string_view message, const std::source_location logLocation)
	{
		// CLion shows this as an error, I don't know why because it links and runs just fine.
		const std::filesystem::path full_path(logLocation.file_name());
		const std::string output = std::format("[{}:{}] {}", full_path.filename().string(), logLocation.line(), message);

		log_internal(Info, output);
	}

	void Logger::log_warning(const std::string_view message, const std::source_location logLocation) {
		const std::filesystem::path full_path(logLocation.file_name());
		const std::string output = std::format("[{}:{}] {}", full_path.filename().string(), logLocation.line(), message);

		log_internal(Warning, output);
	}

	void Logger::log_error(const std::string_view message, const std::source_location logLocation) {
		const std::filesystem::path full_path(logLocation.file_name());
		const std::string output = std::format("[{}:{}] {}", full_path.filename().string(), logLocation.line(), message);

		log_internal(Error, output);
	}

	void Logger::log_debug(const std::string_view message, const std::source_location logLocation) {
		const std::filesystem::path full_path(logLocation.file_name());
		const std::string output = std::format("[{}:{}] {}", full_path.filename().string(), logLocation.line(), message);

		log_internal(Debug, output);
	}

	std::string level_to_string(const LogLevel level) {
		switch (level) {
			case Info:
				return "Info";
			case Warning:
				return "Warning";
			case Error:
				return "Error";
			case Debug:
				return "Debug";
		}
		return "Unknown";
	};

	void Logger::log_no_format(const LogLevel level, const std::string_view message) {
		log_internal(level, message.data());
	}

	void Logger::log_internal(const LogLevel level, const std::string& message) {
		const auto levelString = level_to_string(level);
		const std::string output = std::format("[{}] {}", levelString, message);
		ConsoleService::log(level, output);

		if (level == Error) {
			std::println(stderr, "{}", output);
		} else {
			// eurgh
			std::println("{}", output);
		}
	}
}
