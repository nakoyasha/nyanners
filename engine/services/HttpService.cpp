#include "./HttpService.h"

int HttpService::luaIndex(lua_State* context, const std::string property)
{
    if (property == "request") {
        reflection_luaPushMethod(context, [this](lua_State* context) {
            // std::string url = luaL_checkstring(context, -1);
            // std::string path = luaL_checkstring(context, -2);

            HttpResponse response;
            httplib::Result result;
            try {
                result = this->request(HttpMethod::GET, "https://roblox.com", "/");
                lua_pushstring(context, response.body.c_str());
                return 1;
            } catch (httplib::Error err) {
                switch (err) {
                case httplib::Error::Connection: {
                    lua_throwError(context, std::string("Error establishing a connection"));
                    break;
                };
                case httplib::Error::Canceled: {
                    lua_throwError(context, "Request cancelled");
                    break;
                };
                case httplib::Error::ConnectionTimeout: {
                    lua_throwError(context, "Connection timed out before server could send a response");
                    break;
                }
                case httplib::Error::SSLConnection: {
                    lua_throwError(context, "SSL Connection failure");
                    break;
                }
                case httplib::Error::SSLLoadingCerts: {
                    lua_throwError(context, "Loading SSL certs failed");
                    break;
                }
                case httplib::Error::SSLServerVerification: {
                    lua_throwError(context, "Server could not be verified thus no trusted connection could be established");
                    break;
                }
                case httplib::Error::SSLServerHostnameVerification: {
                    lua_throwError(context, "Server hostname does not match certificate and thus no trusted connection could be established");
                    break;
                }
                case httplib::Error::Compression: {
                    lua_throwError(context, "Server uses unknown compression scheme");
                    break;
                }
                case httplib::Error::Read: {
                    lua_throwError(context, "Error while reading");
                    break;
                }
                case httplib::Error::ProxyConnection: {
                    lua_throwError(context, "Proxy connection failrue");
                    break;
                }
                case httplib::Error::Unknown: {
                    lua_throwError(context, "Unknown error");
                    break;
                }
                case httplib::Error::Success: {
                    lua_pushstring(context, response.body.c_str());
                    break;
                }
                default: {
                    lua_throwError(context, httplib::to_string(err));
                    break;
                }
                }
                return 1;
            }
        });
        return 1;
    } else {
        return Instance::luaIndex(context, property);
    }

    return 0;
}

std::string& trim(std::string& string, char character, bool reverse = false)
{
    return reverse
        ? string.erase(string.find_last_not_of(character) + 1)
        : string.erase(0, string.find_first_not_of(character));
}

auto makeHttpRequest(HttpMethod method, const std::string url, const std::string path)
{

    return httplib::Result();
}

httplib::Result HttpService::request(HttpMethod method, const std::string url, const std::string path)
{
    httplib::Client client(url);

    if (method == HttpMethod::GET) {
        return client.Get(path);
    } else if (method == HttpMethod::POST) {
        return client.Post(path);
    }
}