#include "./HttpService.h"

int HttpService::luaIndex(lua_State* context, const std::string property)
{
    if (property == "request") {
        reflection_luaPushMethod(context, [this](lua_State* context) {
            // for some reason, -1 means "the 2nd argument" here
            // instead of the first one, so they have to be flipped...
            // is this a luau bug?? or am i dumb?
            std::string url = luaL_checkstring(context, -2);
            std::string path = luaL_checkstring(context, -1);

            HttpResponse response;
            httplib::Result result;

            result = this->request(HttpMethod::GET, url, path);

            if (result.error() == httplib::Error::Success) {
                response.code = result->status;
                response.body = result->body;
                reflection_luaPushStruct(context, { { "code", LuaValue(response.code) }, { "body", response.body } });
                return 1;
            } else {
                lua_throwError(context, httplib::to_string(result.error()));
                return 0;
            }
        });
        return 1;
    } else if (property == "JSONDecode") {
        reflection_luaPushMethod(context, [this](lua_State* context) {
            std::string jsonString = luaL_checkstring(context, -1);
            nlohmann::json parsed = nlohmann::json::parse(jsonString);

            lua_newtable(context);
            int stackTop = lua_gettop(context);

            for (auto [key, value] : parsed.items()) {

                if (value.is_string()) {
                    std::string val = value.get<std::string>();
                    lua_pushlstring(context, val.c_str(), val.size());
                }

                if (value.is_number_integer()) {
                    lua_pushnumber(context, value.get<int>());
                }

                if (value.is_number_float()) {
                    lua_pushnumber(context, value.get<float>());
                }

                if (value.is_boolean()) {
                    lua_pushboolean(context, value.get<bool>());
                }

                lua_setfield(context, stackTop, key.c_str());
            }

            return 1;
        });
        return 1;
    } else {
        return Instance::luaIndex(context, property);
    }

    return 0;
}

auto makeHttpRequest(HttpMethod method, const std::string url, const std::string path)
{

    return httplib::Result();
}

httplib::Result HttpService::request(HttpMethod method, const std::string url, const std::string path)
{
    httplib::SSLClient client(url);
    client.enable_server_certificate_verification(true);
    client.set_follow_location(true);

    if (method == HttpMethod::GET) {
        return client.Get(path);
    } else if (method == HttpMethod::POST) {
        return client.Post(path);
    }
}