#include <iostream>
#include "ConsoleLogger.h"
#include "HttpConnection.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

namespace gorilla 
{
namespace http
{

void ConsoleLogger::_handle(const HttpConnectionPtr& conn, const Callback& next)
{
    HttpRequestPtr req = conn->req;
    //do not capture conn because it will cause circular reference!
    conn->res->addNotify([req](HttpResponse* res){
        auto tFinish = boost::posix_time::microsec_clock::universal_time();
        boost::posix_time::time_duration td = tFinish - req->tCreate;
        std::cout << req->ip << " " << req->method << " "
                  << req->originalUrl << " " << (int)res->statusCode
                  << " " << td.total_milliseconds()
                  << "ms" << std::endl;
    });
    next(nullptr);
}

} //http
} //gorilla
