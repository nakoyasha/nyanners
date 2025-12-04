#pragma once

#include "instances/Instance.h"
#include "lua/reflection/Reflection.h"

#if defined(_WIN32)
#define NOGDI             // All GDI defines and routines
#define NOUSER            // All USER defines and routines
#endif
#include "curl/curl.h"
#include "http/json.hpp"

enum HttpMethod {
    GET,
    POST,
};

struct HttpResponse {
    int code;
    CURLcode curlStatus;
    std::string body;
};

namespace Nyanners::Instances {
class HttpService : public Instance {
public:
    HttpService();

    int luaIndex(lua_State* context, const std::string property);
    HttpResponse request(HttpMethod method, const std::string url);
    CURL* curlInstance = nullptr;
private:
    // taken from https://gist.github.com/whoshuu/2dc858b8730079602044 (shamlessly)
    static std::size_t curlAllocateResponse(void* ptr, std::size_t size, std::size_t nmemb, std::string* data);
};
}