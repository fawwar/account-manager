#pragma once
#include "IRequestHandler.h"

namespace gorilla 
{
namespace http
{
/*
    access file according baseDir + path
    Here, path is url router processed path
    TODO: ETag, Last-Modified, partial range
*/
class StaticFileHandler: public IRequestHandler
{
    std::string baseDir;
public:
    StaticFileHandler(const std::string& baseDir);
    ~StaticFileHandler();
    void _handle(const HttpConnectionPtr& conn, const Callback& next);
};

} //http
} //gorilla
