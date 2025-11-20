#pragma once

#include "instances/Instance.h"
#include "lua/system.h"

enum HttpMethod {
    GET,
    POST,
};

struct HttpResponse {
    int code;
    std::string body;
};

namespace Nyanners::Instances {
class HttpService : public Instance {
public:
    HttpService()
        : Instance("HttpService") { };

    int makeLuaRequest(lua_State* context);
    int luaIndex(lua_State* context, const std::string property);
    HttpResponse request(HttpMethod method, const std::string url);
};
}