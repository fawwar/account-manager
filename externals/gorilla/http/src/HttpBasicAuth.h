#pragma once
#include <string>
#include "HttpDataItem.h"

namespace gorilla
{
namespace http
{
/**
    BasicAuthHandler will store basic auth info to req->httpDataManager
*/
class HttpBasicAuth: public HttpDataItem
{
public:
    std::string realm;
    std::string username;
    std::string password;
    
    bool isValid(); //true if username is not null
    std::string toString() const;
};

} //http
} //gorilla
