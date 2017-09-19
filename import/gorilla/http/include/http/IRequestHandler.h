#pragma once
#include <exception>
#include "ptr.h"

namespace gorilla 
{
namespace http
{

/**
    middleware or handler MUST inherit this interface and implement _handle
*/
class IRequestHandler
{
public:
    typedef std::shared_ptr<IRequestHandler> ptr;
    typedef std::function<void(std::exception_ptr e)> Callback;
    void handle(const HttpConnectionPtr& conn, const Callback& next);
    virtual ~IRequestHandler();
protected:
    /*
    'next' can be defered
    ignore 'next' if response handler ends here
    call next(nullptr); to forward request without error
    call next(some std exception shared_ptr) to report error and stops forwarding request
    throw std::exception is similar
    */
    virtual void _handle(const HttpConnectionPtr& conn, const Callback& next) = 0;
};

} //http
} //gorilla
