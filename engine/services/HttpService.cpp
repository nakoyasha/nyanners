#include "./HttpService.h"

int HttpService::luaIndex(lua_State* context, const std::string property)
{
    if (property == "request") {
        reflection_luaPushMethod(context, [this](lua_State* context) {
            HttpResponse response = this->request(HttpMethod::GET, "https://google.com");
            lua_pushstring(context, response.body.c_str());
            return 1;
        });
        return 1;
    } else {
        return Instance::luaIndex(context, property);
    }

    return 0;
}

HttpResponse HttpService::request(HttpMethod method, const std::string url)
{
    // TODO: implement actual http
    HttpResponse response;
    response.code = 200;
    response.body = "{success: true, message: \"yay\"}";

    return response;
}