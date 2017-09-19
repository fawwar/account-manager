#pragma once
#include <map>
#include "ptr.h"

namespace gorilla
{
namespace http
{
/**
    key value store used in req->httpDataManager
    this is a temporary store between the handlers of the same request.
*/
class HttpDataManager
{
    std::map<std::string, HttpDataItemPtr> httpData;
public:
    //get will return nullptr if key is not found
    HttpDataItemPtr get(const std::string& key);
    std::string getString(const std::string& key);
    void set(const std::string& key, const HttpDataItemPtr& value);
    void set(const std::string& key, const std::string& value);
};

} //http
} //gorilla
