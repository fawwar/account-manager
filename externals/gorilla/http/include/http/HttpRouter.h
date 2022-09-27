#pragma once
#include <string>
#include <utility>//std::pair
#include <list>
#include <functional>
#include <boost/tuple/tuple.hpp>
#include <boost/regex.hpp>
#include "IRequestHandler.h"

#define BIND_REQUEST(X) std::bind(X, this, std::placeholders::_1, std::placeholders::_2)

namespace gorilla 
{
namespace http
{
/**
    do url routing
    
*/

class HttpRouter: public IRequestHandler
{
public:
    typedef std::function<void(const HttpConnectionPtr&, const Callback&)> handler_t;
    /*
    path supports exact match, name extraction, regex
    name extraction use :name or {name}
    regex use [regex] or [regex]:name
    NOT support dynamic registering handlers! SHOULD add handler before server starts
    eg.
      "/api/test" matches /api/test
      "/api/:name" matches /api/123, name=123
      "/{name}/{value}" matches /api/test, name=api, value=test     
      "/[(\\d+)x(\\d+)]" matches /123x456 group1=123, group2=456
      "/[(\\d+)x(\\d+)]:id" matches /123x456 group1=123, group2=456, id=123x456
    note:
      regex must use whole segment:
      "/[regex]/[regex]/{name}" is ok
      "/head[regex]/[regex]tail" is not ok
      similarly
      "/api/{name}" is ok
      "/api/head{name}" is not ok
    */
    
    //use is for http middleware
    //if path is not provided, handler will be matched to all url path
    //pass lambda function(handler_t) or object which inherits IRequestHandlerPtr)
    //HttpRouter also inherits IRequestHandler and can be chained.
    
    //handler object can be used multiple times IF STATELESS
    //stateless means no internal state
    //HttpRouter is stateless
    
    HttpRouter* use(handler_t handler);
    HttpRouter* use(const IRequestHandlerPtr& handler);
    HttpRouter* use(const std::string& path, handler_t handler);
    HttpRouter* use(const std::string& path, const IRequestHandlerPtr& handler);
    
    //support get,post,del,put. for other verbs, use 'use' and handle method inside
    HttpRouter* get(const std::string& path, handler_t handler);
    HttpRouter* get(const std::string& path, const IRequestHandlerPtr& handler);
    
    HttpRouter* head(const std::string& path, handler_t handler);
    HttpRouter* head(const std::string& path, const IRequestHandlerPtr& handler);
    
    HttpRouter* post(const std::string& path, handler_t handler);
    HttpRouter* post(const std::string& path, const IRequestHandlerPtr& handler);

    //delete is reserved keyword in C++, use del instead
    HttpRouter* del(const std::string& path, handler_t handler);
    HttpRouter* del(const std::string& path, const IRequestHandlerPtr& handler);
    
    HttpRouter* put(const std::string& path, handler_t handler);
    HttpRouter* put(const std::string& path, const IRequestHandlerPtr& handler);
    
    HttpRouter* options(const std::string& path, handler_t handler);
    HttpRouter* options(const std::string& path, const IRequestHandlerPtr& handler);
    
protected:
    void _handle(const HttpConnectionPtr& conn, const Callback& next);
    
    HttpRouter* addHandler(const std::string& method, const std::string& path, handler_t handler);
    HttpRouter* addHandler(const std::string& method, const std::string& path, const IRequestHandlerPtr& handler);

    //method, path, handler
    typedef std::list<boost::tuple<std::string, std::string, handler_t> > handlers_t;
    
    handlers_t handlers;
    
    void processNext(handlers_t::iterator it, const HttpConnectionPtr& conn, const Callback& next);
};

} //http
} //gorilla
