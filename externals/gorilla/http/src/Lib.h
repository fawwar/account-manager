#pragma once
#include <boost/network/include/http/server.hpp>

namespace gorilla 
{
namespace http
{
//forward declaration gorilla::http::HttpServer
class HttpServer; 
/**
    simple typedef to reduce third-party lib name dependency
*/
namespace Lib
{
    //gorilla::http::HttpServer is also the handler class for cpp-netlib http server
    typedef boost::network::http::async_server<gorilla::http::HttpServer> HttpServer;
    typedef HttpServer::connection_ptr HttpConnection;
    typedef HttpServer::connection::status_t HttpStatusCode;
    typedef HttpServer::request HttpRequest;
    typedef boost::network::http::request_header_narrow HttpRequestHeader;
}

} //http
} //gorilla
