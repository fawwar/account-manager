#include "HttpDataManager.h"
#include "HttpDataItem.h"

namespace gorilla
{
namespace http
{

HttpDataItemPtr HttpDataManager::get(const std::string& key)
{
    if(httpData.find(key) != httpData.end())
    {
        return httpData[key];
    }
    return nullptr;
}
std::string HttpDataManager::getString(const std::string& key)
{
    if(httpData.find(key) != httpData.end())
    {
        return httpData[key]->toString();
    }
    return "";
}
void HttpDataManager::set(const std::string& key, const HttpDataItemPtr& value)
{
    httpData[key] = value;
}
class HttpDataItemString: public HttpDataItem
{
  std::string str;
public:
  std::string toString() const
  {
      return str;
  }
  HttpDataItemString(const std::string& s)
  {
      str = s;
  }
};
void HttpDataManager::set(const std::string& key, const std::string& value)
{
    httpData[key] = std::make_shared<HttpDataItemString>(value);
}

} //http
} //gorilla
