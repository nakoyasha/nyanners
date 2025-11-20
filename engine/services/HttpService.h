#pragma once

#include "instances/Instance.h"
#include "lua/system.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "http/httplib.h"
#include "http/json.hpp"

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

    int luaIndex(lua_State* context, const std::string property);
    httplib::Result request(HttpMethod method, const std::string url, const std::string path);
};
}