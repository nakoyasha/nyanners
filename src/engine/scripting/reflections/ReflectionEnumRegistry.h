#pragma once
#include "lua.h"
#include <map>
#include <string>
#include <type_traits>

namespace Nyanners::Scripting::Reflection {
	class ReflectionEnumRegistry {
	public:
		static ReflectionEnumRegistry &instance() {
			static ReflectionEnumRegistry registry;
			return registry;
		}

		template <typename Enum>
		void register_enum(
		  const std::string &name, const std::map<std::string, Enum> &values
		) {
			static_assert(std::is_enum_v<Enum>);
			auto &reflected = enums[name];
			for (const auto &[itemName, item] : values) {
				reflected[itemName] = static_cast<int>(item);
			}
		}

		void register_values(
		  const std::string &name, const std::map<std::string, int> &values
		);

		[[nodiscard]] const std::map<std::string, int> *find_values(
		  const std::string &name
		) const;

		void push_to_lua(lua_State *context) const;

	private:
		std::map<std::string, std::map<std::string, int>> enums;
	};
}
