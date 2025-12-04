#include "./HttpService.h"

#include "engine.h"
#include "lua/system.h"

void createJSONLuaObject(lua_State* context, nlohmann::json json)
{
    lua_newtable(context);
    int stackTop = lua_gettop(context);

    for (auto [key, value] : json.items()) {

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

        if (value.is_object() || value.is_array()) {
            createJSONLuaObject(context, value);
        }

        lua_setfield(context, stackTop, key.c_str());
    }
}

int HttpService::luaIndex(lua_State* context, const std::string property)
{
    if (property == "request") {
        reflection_luaPushMethod(context, [this](lua_State* context) {
            // for some reason, -1 means "the 2nd argument" here
            // instead of the first one, so they have to be flipped...
            // is this a luau bug?? or am i dumb?
            std::string url = luaL_checkstring(context, -1);

            HttpResponse response = this->request(HttpMethod::GET, url);

            if (response.curlStatus != CURLE_OK) {
                std::string error = curl_easy_strerror(response.curlStatus);
                lua_throwError(context, error);
                return 0;
            }

            reflection_luaPushStruct(context, {
                { "code", response.code },
                { "body", std::string(response.body) } }
                );

            return 1;
        });
        return 1;
    } else if (property == "JSONDecode") {
        reflection_luaPushMethod(context, [this](lua_State* context) {
            std::string jsonString = luaL_checkstring(context, -1);
            nlohmann::json parsed = nlohmann::json::parse(jsonString);

            createJSONLuaObject(context, parsed);

            return 1;
        });
        return 1;
    } else {
        return Instance::luaIndex(context, property);
    }

    return 0;
}

HttpService::HttpService() : Instance("HttpService")
{
    curlInstance = curl_easy_init();

    if (!curlInstance) {
        Application::instance().panic("libcurl initialization failed");
    }

    curl_easy_setopt(curlInstance, CURLOPT_CAINFO, "certs/curl-ca-bundle.crt");

    curl_version_info_data* data = curl_version_info(CURLVERSION_NOW);
    std::cout << "libcurl backend " << data->ssl_version << std::endl;
}

std::size_t HttpService::curlAllocateResponse(void* ptr, std::size_t size, std::size_t nmemb, std::string* data)
{
    data->append((char*)ptr, size * nmemb);
    return size * nmemb;
}

HttpResponse HttpService::request(HttpMethod method, const std::string url)
{
    int responseCode = -1;
    // double responseTime = 0.0;
    std::string response_string;
    // std::string header_string;

    // setup headers
    curl_easy_setopt(curlInstance, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curlInstance, CURLOPT_USERAGENT, "nyanners/v0.0.1");

    curl_easy_setopt(curlInstance, CURLOPT_WRITEFUNCTION, curlAllocateResponse);
    curl_easy_setopt(curlInstance, CURLOPT_WRITEDATA, &response_string);
    CURLcode curlStatus = curl_easy_perform(curlInstance);

    if (curlStatus == CURLE_OK) {
        curl_easy_getinfo(curlInstance, CURLINFO_RESPONSE_CODE, &responseCode);
        // curl_easy_getinfo(curlInstance, CURLINFO_TOTAL_TIME, &responseTime);
    }
    curl_easy_cleanup(curlInstance);

    return {(int)responseCode, curlStatus, response_string};
}