#include <boost/algorithm/string.hpp>
#include "HttpRouter.h"
#include "HttpServer.h"
#include "HttpConnection.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

namespace gorilla 
{
namespace http
{

//file scope function, only used by matchUrl
static bool matchRegexId(const std::string& pattern, std::string& regexPart, std::string& idPart)
{
    //match "[regex]:id"
    boost::regex expr{"\\[(.+)\\]\\:(\\w+)"};
    boost::smatch what;
    if(boost::regex_match(pattern, what, expr))
    {
        regexPart = what[1];
        idPart = what[2];
        return true;
    }
    else if(pattern.length() > 2 && pattern.front() == '[' && pattern.back() == ']' )
    {
        regexPart = pattern.substr(1, pattern.length() - 2);
        idPart = "";
        return true;            
    }
    return false;
}

//match url: handler http verb, handler path pattern, req will be modified if matchUrl return true
static bool matchUrl(const std::string& method, const std::string& pattern, const HttpRequestPtr& req)
{
    std::string path = req->paths.top();
    if(method != "" && method != req->method) return false;
    std::vector<std::string> v1; //pattern
    std::vector<std::string> v2; //path
    std::map<std::string, std::string> kv;//for route params
    std::map<std::pair<int,int>, std::string> kv2;
    //boost::algorithm::token_compress_on
    boost::split(v1, pattern, boost::is_any_of("/"));
    if(path.empty())
    {
      boost::split(v2, "/", boost::is_any_of("/"));
    }
    else
    {
      boost::split(v2, path, boost::is_any_of("/"));
    }
    if(v1.size() > v2.size()) return false; //currently no optional params
    if(v1.size() < v2.size() && method != "")
    {
        //means not 'use()', should match full path
        return false;
    }
    int len1 = v1.size();
    int len2 = v2.size();
    int regCount = 0;
    for(int i = 0; i < len1; i++)
    {
        std::string idPart, regexPart;
        if(v1[i] == v2[i]) continue;
        if(v1[i].empty() && i == len1 - 1 && method == "")
        {
          v1.pop_back();
          len1 = v1.size();
          continue;
        }
        if(v1[i].length() > 1 && v1[i].front() == ':') //kv requires name
        {
            kv[v1[i].substr(1)] = v2[i];
            continue;
        }
        else if(v1[i].length() > 2 && v1[i].front() == '{' && v1[i].back() == '}')
        {
            kv[v1[i].substr(1, v1[i].length() - 2)] = v2[i];
            continue;
        }
        else if(matchRegexId(v1[i], regexPart, idPart))
        {
            //do regex match
            std::cout << "parsed: " << regexPart << "," << idPart << std::endl;
            boost::regex expr{regexPart};
            boost::smatch what;
            if(boost::regex_match(v2[i], what, expr))
            {
               if(!idPart.empty())
               {
                   kv[idPart] = what[0];
               }
               int len = what.size();
               for(int i = 0; i < len; i++)
               {
                   kv2[std::pair<int, int>(regCount, i)] = what[i];                   
               }
               regCount++;
               continue;
            }
        }
        return false;
    }    
    
    if(req->paths.size() == 1)
    {
      req->routeParams.clear();
    }
    for(auto& it: kv)
    {
      req->routeParams.set(it.first, it.second);
    }
    for(auto& it: kv2)
    {
      req->routeParams.set(it.first.first, it.first.second, it.second);
    }
    std::string matchedPath;
    
    for(int i = len1; i < len2; i++)
    {
        matchedPath += "/";        
        matchedPath += v2[i]; 
    }
    if(path.empty())
    {
        matchedPath = "/";
    }
    req->paths.push(matchedPath);
    return true;    
}

    
void HttpRouter::processNext(handlers_t::iterator it, const HttpConnectionPtr& conn, const Callback& next)
{
    if(it == this->handlers.end()) //no furthur handler
    {
        next(nullptr);
        return;
    }
    std::string& method = it->get<0>();
    std::string& pattern = it->get<1>();
    handler_t& handler = it->get<2>();
    ++it;
    //need to catch exception here because matchUrl might throw
    try
    {
        if(matchUrl(method, pattern, conn->req))
        {
            handler(conn, [it, this, conn, next](std::exception_ptr e) {
                conn->req->paths.pop();
                if(e)
                {
                    next(e);
                }
                else
                {
                    conn->io_service_ptr->post([it, this, conn, next]() {
                        this->processNext(it, conn, next);
                    });
                }
            });
        }
        else
        {
            conn->io_service_ptr->post([it, this, conn, next]() {
                this->processNext(it, conn, next);
            });
        }
    }
    catch(std::exception& e)
    {
        next(std::current_exception());
    }
}

void HttpRouter::_handle(const HttpConnectionPtr& conn, const Callback& next)
{
    auto it = handlers.begin();
    processNext(it, conn, next);
}

HttpRouter* HttpRouter::use(handler_t handler)
{
    return addHandler("", "/", handler);
}

HttpRouter* HttpRouter::use(const IRequestHandlerPtr& handler)
{
    return addHandler("", "/", handler);
}

HttpRouter* HttpRouter::use(const std::string& path, handler_t handler)
{
    return addHandler("", path, handler);
}

HttpRouter* HttpRouter::use(const std::string& path, const IRequestHandlerPtr& handler)
{
    return addHandler("", path, handler);
}

HttpRouter* HttpRouter::get(const std::string& path, handler_t handler)
{
    return addHandler("GET", path, handler);
}

HttpRouter* HttpRouter::get(const std::string& path, const IRequestHandlerPtr& handler)
{
    return addHandler("GET", path, handler);
}

HttpRouter* HttpRouter::head(const std::string& path, handler_t handler)
{
    return addHandler("HEAD", path, handler);
}

HttpRouter* HttpRouter::head(const std::string& path, const IRequestHandlerPtr& handler)
{
    return addHandler("HEAD", path, handler);
}

HttpRouter* HttpRouter::post(const std::string& path, handler_t handler)
{
    return addHandler("POST", path, handler);
}

HttpRouter* HttpRouter::post(const std::string& path, const IRequestHandlerPtr& handler)
{
    return addHandler("POST", path, handler);
}

HttpRouter* HttpRouter::del(const std::string& path, handler_t handler)
{
    return addHandler("DELETE", path, handler);
}

HttpRouter* HttpRouter::del(const std::string& path, const IRequestHandlerPtr& handler)
{
    return addHandler("DELETE", path, handler);
}

HttpRouter* HttpRouter::put(const std::string& path, handler_t handler)
{
    return addHandler("PUT", path, handler);
}

HttpRouter* HttpRouter::put(const std::string& path, const IRequestHandlerPtr& handler)
{
    return addHandler("PUT", path, handler);
}

HttpRouter* HttpRouter::options(const std::string& path, handler_t handler)
{
    return addHandler("OPTIONS", path, handler);
}

HttpRouter* HttpRouter::options(const std::string& path, const IRequestHandlerPtr& handler)
{
    return addHandler("OPTIONS", path, handler);
}


HttpRouter* HttpRouter::addHandler(const std::string& method, const std::string& path, handler_t handler)
{
    handlers.push_back(boost::make_tuple(method, path, handler));
    return this;
}

HttpRouter* HttpRouter::addHandler(const std::string& method, const std::string& path, const IRequestHandlerPtr& handler)
{
    handlers.push_back(boost::make_tuple(method, path, [handler](const HttpConnectionPtr& conn, const Callback& next){
        return handler->handle(conn, next);
    }));
    return this;
}



    
} //http
} //gorilla
