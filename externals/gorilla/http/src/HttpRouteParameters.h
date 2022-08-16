#pragma once
#include <map>
#include <string>

namespace gorilla 
{
namespace http
{
/**
    HttpRouter will store key value or regex grouping result to HttpRouteParameters
*/
class HttpRouter;
class HttpRouteParameters
{
    friend class HttpRouter;
    std::map<std::string, std::string> params;
    std::map<std::pair<int,int>, std::string> regexs;
public:
    //used by HttpRouter, matchUrl
    void set(const std::string& name, const std::string& value);
    void set(const std::map<std::string, std::string>& params);
    void set(int index1, int index2, const std::string& value);
    void set(const std::map<std::pair<int,int>, std::string>& regexs);
    void clear();

    std::string get(const std::string& name);//for :name or {name} matching
    //regexp
    //index is the regex position index (from 0), return the nth regex matched segment
    std::string get(int index);
    //index2 is the group index (starts from 1)
    //return the matched segment if index2=0
    std::string get(int index1, int index2);
};

} //http
} //gorilla
