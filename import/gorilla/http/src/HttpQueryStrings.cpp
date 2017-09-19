#include <boost/algorithm/string.hpp>
#include "HttpQueryStrings.h"
#include "HttpUtil.h"

namespace gorilla 
{
namespace http
{

void HttpQueryStrings::assign(const std::string& queryString)
{
    this->queryString = queryString;
    std::vector<std::string> queryStrings;
    boost::split(queryStrings, queryString ,boost::is_any_of("&"));
    for(auto&& qs : queryStrings)
    {
        auto kv = HttpUtil::split2(qs, '=');
        params[HttpUtil::urlDecode(kv.first)].push_back(HttpUtil::urlDecode(kv.second));
    }
}

bool HttpQueryStrings::contains(const std::string& name) const
{
    auto it = params.find(name);
    return it != params.end();
}

std::string HttpQueryStrings::get(const std::string& name) const
{
    auto& v = getAll(name);
    return v.size() ? v[0] : "";
}

const std::vector<std::string>& HttpQueryStrings::getAll(const std::string& name) const
{
    static const std::vector<std::string> emptyVector;
    auto it = params.find(name);
    if(it == params.end()) return emptyVector;
    return it->second;
}

const std::map<std::string, std::vector<std::string> >& HttpQueryStrings::getAll() const
{
    return params;
}

} //http
} //gorilla
