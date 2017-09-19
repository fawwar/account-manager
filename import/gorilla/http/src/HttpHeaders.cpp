#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <sstream>
#include "HttpHeaders.h"
#include "HttpUtil.h"

namespace gorilla 
{
namespace http
{

void HttpHeaders::setHeaders(const std::vector<Lib::HttpRequestHeader>& headers)
{
    for(auto&& kv: headers)
    {
        appendHeader(kv.name, kv.value);
    }
}

void HttpHeaders::setHeaders(const std::vector<std::string>& headers)
{
    for(auto&& kv: headers)
    {
        appendHeader(kv);
    }
}

void HttpHeaders::setHeaders(const std::string& headers)
{
    std::vector<std::string> v;
    boost::split(v, headers, boost::is_any_of("\n"));
    setHeaders(v);
}

void HttpHeaders::setHeaders(const Json::Value& jheaders)
{
    if(jheaders.isObject())
    {
        for(auto it = jheaders.begin(); it!= jheaders.end(); it++)
        {
            auto v = jheaders[it.name()];
            if(v.isArray())
            {
                for(auto it2 = v.begin(); it2!= v.end(); it2++)
                {
                    appendHeader(it.name(), (*it2).asString());
                }
            }
            else
            {
                setHeader(it.name(), v.asString());
            }
        }
    }
}

void HttpHeaders::setHeader(const std::string& name, const Json::Value& value)
{
    if(name.empty()) return;
    //will clean old values
    std::string k = boost::to_lower_copy(name);
    Header v(name, value.asString());
    headers[k] = std::vector<Header>{v};
}
/*
void HttpHeaders::setHeader(const std::string& name, const std::string& value)
{
    if(name.empty()) return;
    //will clean old values
    std::string k = boost::to_lower_copy(name);
    Header v(name, value);
    headers[k] = std::vector<Header>{v};
}

void HttpHeaders::setHeader(const std::string& name, int value)
{
    setHeader(name, boost::lexical_cast<std::string>(value));
}
*/
void HttpHeaders::setHeader(const std::string& header)
{
    setHeader(HttpUtil::split2(header, ':'));
}

void HttpHeaders::setHeader(const Header& header)
{
    setHeader(header.first, header.second);
}

void HttpHeaders::appendHeader(const std::string& name, const std::string& value)
{
    if(name.empty()) return;
    std::string k = boost::to_lower_copy(name);
    Header v(name, value);
    headers[k].push_back(v);
}

void HttpHeaders::appendHeader(const std::string& name, int value)
{
    appendHeader(name, boost::lexical_cast<std::string>(value));
}

void HttpHeaders::appendHeader(const std::string& header)
{
    appendHeader(HttpUtil::split2(header, ':'));
}

void HttpHeaders::appendHeader(const Header& header)
{
    appendHeader(header.first, header.second);
}

void HttpHeaders::removeHeader(const std::string& name)
{
    headers.erase(boost::to_lower_copy(name));
}

std::string HttpHeaders::get(const std::string& name) const
{
    auto it = headers.find(boost::to_lower_copy(name));
    if(it == headers.end()) return std::string();
    if(it->second.size() == 0) return std::string();
    return it->second.at(0).second;//return the first one
}

const std::vector<HttpHeaders::Header>& HttpHeaders::getAll(const std::string& name) const
{
    static const std::vector<Header> emptyVector;
    auto it = headers.find(boost::to_lower_copy(name));
    if(it == headers.end()) return emptyVector;
    return it->second;
}

const std::map<std::string, std::vector<HttpHeaders::Header> >& HttpHeaders::getAll() const
{
    return headers;
}
bool HttpHeaders::contains(const std::string& name) const
{
    auto it = headers.find(boost::to_lower_copy(name));
    if(it == headers.end()) return false;
    if(it->second.size() == 0) return false;
    return true;
}
void HttpHeaders::setDateNow()
{
    boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
    boost::posix_time::time_facet *facet = new boost::posix_time::time_facet("%a, %d %b %Y %H:%M:%S GMT");
    std::ostringstream is;
    is.imbue(std::locale(is.getloc(), facet));
    is << now;
    setHeader("Date", is.str());
}
std::string HttpHeaders::getContentType() const
{
    auto v = HttpUtil::split2(get("Content-type"), ';');
    return v.first;
}

std::string HttpHeaders::getBoundary() const
{
    std::string s = get("Content-type");
    std::size_t pos = s.find_first_of("boundary=");
    if(pos == std::string::npos) return "";
    std::size_t pos2 = s.find_first_of(';', pos);
    size_t len = std::string::npos;
    if(pos2 != std::string::npos)
    {
        len = pos2 - pos - 9; //"boundary="
    }
    return boost::algorithm::trim_copy(s.substr(pos + 9, len));
}

void HttpHeaders::print() const
{
    for(auto&& kv: headers)
    {
        for(auto&& v: kv.second)
        {
            std::cout << v.first << ":" << v.second << std::endl;
        }
    }
}

Json::Value HttpHeaders::toJson() const
{
    Json::Value jheaders(Json::objectValue);
    auto& hs = this->getAll();
    for(auto& kv: hs)
    {
      if(kv.second.size() == 0)
      {
        jheaders[kv.first] = "";
      }
      else if(kv.second.size() == 1)
      {
        jheaders[kv.first] = kv.second[0].second;
      }
      else
      {
        Json::Value v(Json::arrayValue);
        for(auto& it: kv.second)
        {
          v.append(it.second);
        }
        jheaders[kv.first] = v;
      }  
    }
    return jheaders;    
}

std::string HttpHeaders::toString() const
{
    std::stringstream ss;
    for(auto&& kv: headers)
    {
        for(auto&& v: kv.second)
        {
            ss << v.first << ": " << v.second << "\r\n";
        }
    }
    return ss.str();
}

bool HttpHeaders::noContentLength()
{
    if(get("Transfer-Encoding") == "chunked") return true;
    if(get("Content-Encoding") == "gzip") return true;
    if(get("Content-Encoding") == "deflate") return true;
    if(get("Content-Encoding") == "br") return true;
    if(get("Content-Encoding") == "compress") return true;
    return false;
}

void HttpHeaders::clear()
{
    headers.clear();
}

} //http
} //gorilla
