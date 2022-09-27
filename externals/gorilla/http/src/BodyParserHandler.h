#pragma once
#include "IRequestHandler.h"

namespace gorilla 
{
namespace http
{
/**
//IGNORE multipart in body parse

this handles POST/PUT body async read and forward request when finish reading
default behavior: store to HttpRequest::body, no furture parsing(eg. json,xml)
if there is error when reading, clear the HttpRequest::body, and forward the request.

if content type = application/x-www-form-urlencoded
BodyParserHandler will save parsing result to req data "Body"
use req->getData<HttpQueryStrings>("Body") to access

if content type = json or xml
use req->getData<HttpBoostPtree>("Body") to access parsed result
*/
class BodyParserHandler: public IRequestHandler
{
    static const int64_t defaultLimitLength = 128 * 1024; //128KB
    int64_t limitLength;
public:
    BodyParserHandler();
    void _handle(const HttpConnectionPtr& conn, const Callback& next);
};

} //http
} //gorilla
