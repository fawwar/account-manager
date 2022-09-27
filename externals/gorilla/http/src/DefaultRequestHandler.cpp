#include "DefaultRequestHandler.h"

namespace gorilla 
{
namespace http
{

void DefaultRequestHandler::_handle(const HttpConnectionPtr& conn, const Callback& next)
{
    next(nullptr);
}

} //http
} //gorilla
