#include "IRequestHandler.h"

namespace gorilla 
{
namespace http
{

IRequestHandler::~IRequestHandler()
{
}

void IRequestHandler::handle(const HttpConnectionPtr& conn, const Callback& next)
{
    try
    {
        _handle(conn, next);
    }
    catch(std::exception &e)
    {
        next(std::current_exception());
    }
}

} //http
} //gorilla
