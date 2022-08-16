#pragma once
#include "IRequestHandler.h"

namespace gorilla 
{
namespace http
{
/**
    forward every request
*/
class DefaultRequestHandler: public IRequestHandler
{
public:
    void _handle(const HttpConnectionPtr& conn, const Callback& next);
};

} //http
} //gorilla
