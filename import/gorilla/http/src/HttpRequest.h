#pragma once
#include <memory>
#include <stack>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "ptr.h"
#include "Lib.h"
#include "HttpHeaders.h"
#include "HttpQueryStrings.h"
#include "HttpRouteParameters.h"
#include "HttpDataManager.h"

namespace gorilla 
{
namespace http
{
/**
    provides request info
    path, method..etc
    headers, querystring, routeparams, data
*/
class HttpServer;
class HttpRequest: public std::enable_shared_from_this<HttpRequest>
{
    friend class HttpServer;
    HttpRequest();
    void assign(const Lib::HttpRequest& req); //initialize HttpRequest from cpplib request
public:
    //connection is weakptr because conn has req ptr
    HttpConnectionWeakPtr connection;
    std::stack<std::string> paths; //path stack used by HttpRouter
    std::exception_ptr error;//for error handler
    boost::posix_time::ptime tCreate; //obj creation time

    std::string version; //1.0, 1.1
    std::string method; //methods are case sensitive
    std::string originalUrl;//full path, not relative to handler base
    std::string ip;//client ip
    int port;//client port
    std::string path; //path without querystring
    std::string body; //body parser will fill this
    std::string contentType; //headers.getContentType(), without charset
    int64_t contentLength; //parse content length if any
    std::string boundary; //boundary string for multipart
    
    HttpHeaders headers;
    HttpQueryStrings queryStrings;
    HttpRouteParameters routeParams;
     //storage between request handlers
    HttpDataManager httpDataManager;
    
    static HttpRequestPtr create(const HttpConnectionPtr& connection, const Lib::HttpRequest& req);
    ~HttpRequest();
    
    //TODO: chain input filter stream
    void read(int len, std::string& data, std::function<void(boost::system::error_code ec)> callback);
    
    //fetch data from httpDataManager, 
    //create=false, return nullptr if not exists
    //create=true, return default object if not exists
    //default create=true
    template <typename T>
    std::shared_ptr<T> getData(const std::string& name, bool create = true)
    {
        std::shared_ptr<HttpDataItem> p = httpDataManager.get(name);
        if(!p)
        {
            return create ? std::make_shared<T>() : nullptr;
        }
        return std::dynamic_pointer_cast<T>(p);
    }
    
};

} //http
} //gorilla
