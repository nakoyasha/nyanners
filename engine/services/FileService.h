//
// Created by Haruka on 19.02.2026.
//

#pragma once
#include <fstream>

#include "instances/Instance.h"
#include "lua/system.h"

namespace Nyanners::Services {
	class FileService : public Instances::Instance {
	public:
		FileService() : Instance("FileService") {
			this->methods.insert({"readFile", [this](lua_State* context) {
					const std::string path = luaL_checkstring(context, -1);
					try {
						const std::string result = readFile(path);
						lua_pushstring(context, result.c_str());
						return 1;
					} catch (std::runtime_error err) {
						lua_throwError(context, static_cast<std::string>(err.what()).c_str());
					}

					return 0;
			}});

			this->methods.insert({"writeFile", [this](lua_State* context) {
					const std::string path = luaL_checkstring(context, -2);
					const std::string content = luaL_checkstring(context, -1);

					try {
						writeFile(path, content);
						return 0;
					} catch (std::runtime_error err) {
						lua_throwError(context, static_cast<std::string>(err.what()).c_str());
					}

					return 0;
			}});

			this->methods.insert({"fileExists", [this](lua_State* context) {
					const std::string path = luaL_checkstring(context, -1);
					bool result = fileExists(path);

					lua_pushboolean(context, result);
					return 1;
			}});
		};

		bool fileExists(const std::string &path);

		std::string readFile(const std::string &path);
		void writeFile(const std::string &path, const std::string &content);
	private:
	};
}
