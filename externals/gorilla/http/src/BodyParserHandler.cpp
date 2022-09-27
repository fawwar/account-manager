#include <boost/property_tree/xml_parser.hpp>
#include "BodyParserHandler.h"
#include "HttpConnection.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpBoostPtree.h"

namespace gorilla 
{
namespace http
{

BodyParserHandler::BodyParserHandler():
    limitLength(defaultLimitLength)
{
}

void BodyParserHandler::_handle(const HttpConnectionPtr& conn, const Callback& next)
{
    HttpRequestPtr req = conn->req;
    HttpResponsePtr res = conn->res;
    //ignore multipart in body parse
    if(req->contentType == "multipart/form-data")
    {
        next(nullptr);
    }
    else if(req->method == "POST" || req->method == "PUT")
    {
        //fetch the POST body async read
        int64_t len = conn->req->contentLength;
        if(len > 0 && len <= limitLength)
        {
            req->read(len, req->body, [conn, next](boost::system::error_code ec){
                if(ec)
                {
                    conn->req->body.clear();
                    next(std::make_exception_ptr(std::runtime_error("BodyParserHandler read fail")));
                    return;
                }
                else
                {
                    if(conn->req->contentType == "application/x-www-form-urlencoded")
                    {
                        auto qs = std::make_shared<HttpQueryStrings>();
                        qs->assign(conn->req->body);
                        conn->req->httpDataManager.set("Body", qs);
                    }
                    else if(conn->req->contentType == "application/json")
                    {
                        auto p = std::make_shared<HttpBoostPtree>();
                        try
                        {
                          p->readJson(conn->req->body);
                          conn->req->httpDataManager.set("Body", p);
                        }
                        catch(std::exception& e)
                        {
                          std::cout << "BodyParserHandler:" << e.what() << std::endl;
                        }
                    }
                    else if(conn->req->contentType == "text/xml" ||
                        conn->req->contentType == "application/xml")
                    {
                        auto p = std::make_shared<HttpBoostPtree>();
                        try
                        {
                          p->readXml(conn->req->body);                        
                          conn->req->httpDataManager.set("Body", p);
                        }
                        catch(std::exception& e)
                        {
                          std::cout << "BodyParserHandler:" << e.what() << std::endl;
                        }
                    }
                }
                next(nullptr);
            });
        }
        else
        {
            next(nullptr);
        }
    }
    else
    {
        next(nullptr);
    }
}

} //http
} //gorilla
