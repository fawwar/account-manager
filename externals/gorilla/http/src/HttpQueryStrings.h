#pragma once
#include <map>
#include <vector>
#include <string>
#include "HttpDataItem.h"

namespace gorilla 
{
namespace http
{
/**
    parse query string and do urldecode
      
    support same query fields
    eg. 
      a[]=1&a[]=2&a[]=3 => use get("a[]") to access
      a=1&a=2&a=3 => use get("a")
      a[color]=red => use get("a[color]")
    
    can also be used as application/x-www-form-urlencoded parser
    BodyParserHandler will save parsing result to req HttpDataManager
*/
class HttpQueryStrings: public HttpDataItem 
{
    std::string queryString;//original qs, query string
    
    //processed querystring, preserving the order
    std::map<std::string, std::vector<std::string> > params;
public:
    //will parse and decode
    void assign(const std::string& queryString);
    
    bool contains(const std::string& name) const;
    
    std::string get() const;
    
    std::string get(const std::string& name) const;//get the first item or empty string
    const std::vector<std::string>& getAll(const std::string& name) const;
    const std::map<std::string, std::vector<std::string> >& getAll() const;//get all
};

} //http
} //gorilla
