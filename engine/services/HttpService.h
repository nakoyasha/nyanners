#include "Service.h"
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
class HttpService : public Service {
public:
    HttpService()
        : Service("HttpService") { };

    int makeLuaRequest(lua_State* context)
    {
        HttpResponse response = this->request(HttpMethod::GET, "https://google.com");
        lua_pushstring(context, response.body.c_str());
        return 1;
    };

    int luaIndex(lua_State* context, std::string property)
    {
        if (property == "request") {
            // reflection_luaPushMethod(context, std::mem_fn(&HttpService::makeLuaRequest));
            reflection_luaPushMethod(context, [this](lua_State* context) {
                this->makeLuaRequest(context);
                return 1;
            });
        } else {
            return Instance::luaIndex(context, property);
        }
    }

    HttpResponse request(HttpMethod method, std::string url)
    {
        // TODO: implement actual http
        HttpResponse response;
        response.code = 200;
        response.body = "{success: true, message: \"yay\"}";

        return response;
    }
};
}