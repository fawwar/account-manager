#include "BasicAuthHandler.h"
#include "HttpBasicAuth.h"
#include "HttpStatusCode.h"
#include "HttpUtil.h"
#include "HttpConnection.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

namespace gorilla 
{
namespace http
{

void BasicAuthHandler::_handle(const HttpConnectionPtr& conn, const Callback& next)
{
    HttpRequestPtr req = conn->req;
    HttpResponsePtr res = conn->res;
    
    std::string authorization = req->headers.get("Authorization");
    auto info = HttpUtil::split2(authorization, ' ');
    if(info.first == "Basic")
    {
        std::string s = HttpUtil::base64Decode(info.second);
        auto accPwd = HttpUtil::split2(s, ':');
        std::shared_ptr<HttpBasicAuth> basicAuth = std::make_shared<HttpBasicAuth>();
        //if not pass authentication, request is not used anymore(sendUnauthorized)
        basicAuth->realm = realm;
        basicAuth->username = accPwd.first;
        basicAuth->password = accPwd.second;
        req->httpDataManager.set("BasicAuth", basicAuth);
    }
    authenticate(conn, [this, conn, next](bool isOK) {
        if(isOK)
        {
            next(nullptr);
        }
        else
        {
            conn->dropBytes(conn->req->contentLength, [this, conn](boost::system::error_code ec){
              this->sendUnauthorized(conn);
            });
        }
    });
}

void BasicAuthHandler::sendUnauthorized(const HttpConnectionPtr& conn)
{
    HttpRequestPtr req = conn->req;
    HttpResponsePtr res = conn->res;
    //ask for input auth    
    res->setStatusCode(HttpStatusCode::UNAUTHORIZED);
    res->headers.setHeader("WWW-Authenticate", "Basic realm=\"" + realm + "\"");
    res->sendHtml(
        std::string() + 
        "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">"
        "<html><head><title>401 Unauthorized</title></head><body>" 
        "<h1>Unauthorized</h1>"
        "<p>" + req->method + " " +  
        req->originalUrl + "</p>" + 
        "</body></html>");
}

BasicAuthHandler::BasicAuthHandler()
{
    realm = "Gorilla HTTP";    
}

void BasicAuthHandler::authenticate(const HttpConnectionPtr& conn, std::function<void(bool)> cb)
{
    //auth example
    /*
    auto basicAuth = conn->req->getData<HttpBasicAuth>("BasicAuth", true);
    if(basicAuth->username == "admin" && basicAuth->password == "admin")
    {
        cb(true); //default pass always
        return;
    }
    cb(false);
    */
    cb(true);
}


} //http
} //gorilla
