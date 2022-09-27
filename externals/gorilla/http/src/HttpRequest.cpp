#include <boost/lexical_cast.hpp>
#include "HttpRequest.h"
#include "HttpConnection.h"
#include "HttpUtil.h"

namespace gorilla 
{
namespace http
{

HttpRequest::HttpRequest():
    port(0),//client connection port
    contentLength(0)
{
    tCreate = boost::posix_time::microsec_clock::universal_time();
}

HttpRequestPtr HttpRequest::create(const HttpConnectionPtr& connection, const Lib::HttpRequest& req)
{
    HttpRequestPtr sp(new HttpRequest());
    sp->connection = connection;
    sp->assign(req);
    return sp;
}

HttpRequest::~HttpRequest()
{
    //std::cout << "~HttpRequest()" << std::endl;
}

void HttpRequest::assign(const Lib::HttpRequest& req)
{
    version = boost::lexical_cast<std::string>((int)req.http_version_major) + "." + boost::lexical_cast<std::string>((int)req.http_version_minor);
    headers.setHeaders(req.headers);
    contentType = headers.getContentType();
    if(contentType == "multipart/form-data")
    {
      auto cts = HttpUtil::splitn(headers.get("Content-Type"));
      boundary = cts["boundary"];
    }
    if(headers.contains("Content-Length"))
    {
      contentLength = boost::lexical_cast<int64_t>(headers.get("Content-Length"));
    }    
    method = req.method;
    originalUrl = req.destination;
    auto ip_port = HttpUtil::split2(req.source, ':');
    ip = ip_port.first;
    port = req.source_port;
    auto path_query = HttpUtil::split2(originalUrl, '?');    
    path = HttpUtil::urlDecode(path_query.first);
    queryStrings.assign(path_query.second);   
    paths.push(path);
}

void HttpRequest::read(int len, std::string& data, std::function<void(boost::system::error_code ec)> callback)
{
    HttpConnectionPtr conn = connection.lock();
    conn->readBody(len, data, callback);
}

} //http
} //gorilla
