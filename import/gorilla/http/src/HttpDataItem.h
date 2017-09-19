#pragma once
#include <memory>
#include <string>

namespace gorilla
{
namespace http
{
/**
    to store data in req->httpDataManager
    MUST inherit this class
*/
class HttpDataItem
{
public:
    virtual ~HttpDataItem();
    virtual std::string toString() const;
};

} //http
} //gorilla
