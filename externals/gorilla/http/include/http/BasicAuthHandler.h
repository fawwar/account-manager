#pragma once
#include "IRequestHandler.h"

namespace gorilla 
{
namespace http
{
/**
    handle Basic Authentication (rfc7235)
    default behavior: parse auth info and forward the request silently
    SHOULD inherit this class and implement authenticate logic
*/
class BasicAuthHandler: public IRequestHandler
{
protected:
    std::string realm; //"Gorilla HTTP", inherited class can reset
    void _handle(const HttpConnectionPtr& conn, const Callback& next);
    //if cb(false) use sendUnauthorized to send response
    //can be overrided
    virtual void sendUnauthorized(const HttpConnectionPtr& conn);
public:
    BasicAuthHandler();
    //authenticate logic
    //cb: result callback, can be defered
    //ok: cb(true), fail: cb(false)
    virtual void authenticate(const HttpConnectionPtr& conn, std::function<void(bool)> cb);
};

} //http
} //gorilla
