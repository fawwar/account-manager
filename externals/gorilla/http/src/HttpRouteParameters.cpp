#include "HttpQueryStrings.h"
#include "HttpUtil.h"
#include <boost/algorithm/string.hpp>

namespace gorilla 
{
namespace http
{

std::string HttpRouteParameters::get(const std::string& name)
{
    auto it = params.find(name);
    if(it != params.end())
    {
        return it->second;
    }
    return std::string();
}
std::string HttpRouteParameters::get(int index)
{
    return get(index, 0);
}
std::string HttpRouteParameters::get(int index1, int index2)
{
    auto it = regexs.find(std::pair<int,int>(index1, index2));
    if(it != regexs.end())
    {
        return it->second;
    }
    return std::string();
}
void HttpRouteParameters::set(const std::string& name, const std::string& value)
{
    params[name] = value;
}
void HttpRouteParameters::set(const std::map<std::string, std::string>& params)
{
    this->params = params;
}
void HttpRouteParameters::set(int index1, int index2, const std::string& value)
{
    regexs[std::pair<int,int>(index1, index2)] = value;
}
void HttpRouteParameters::set(const std::map<std::pair<int,int>, std::string>& regexs)
{
    this->regexs = regexs;
}
void HttpRouteParameters::clear()
{
    params.clear();
    regexs.clear();
}

} //http
} //gorilla
