#pragma once
#include "IRequestHandler.h"

namespace gorilla 
{
namespace http
{
/**
a simple request/response logger, output to console
*/
class ConsoleLogger: public IRequestHandler
{
public:
    void _handle(const HttpConnectionPtr& conn, const Callback& next);
};

} //http
} //gorilla
