#pragma once
#include "ptr.h"
#include "IOutputStream.h"

namespace gorilla
{
namespace http
{
/**
    forward output to HttpConnection, ignore nextOutput
*/
class ConnOutputStream: public IOutputStream
{
    HttpConnectionWeakPtr connection;
public:
    ConnOutputStream(const HttpConnectionPtr& connection);
    ~ConnOutputStream();
    void write(const std::string& s, std::function<void(boost::system::error_code ec)> callback);
};

} //http
} //gorilla
