#pragma once
#include <utility>
#include <string>
#include "HttpRequest.h"

namespace gorilla 
{
namespace http
{
class HttpUtil
{
public:
    static std::string urlDecode(const std::string& s);
    static std::string urlEncode(const std::string& s);
    //split to two piece, also do trim
    static std::pair<std::string, std::string> split2(const std::string& s, char c);
    static std::map<std::string, std::string> splitn(const std::string& s); //split by ; , assign k=v part, remove quote
    
    static std::string gzipCompress(const std::string& data);
    static std::string gzipDecompress(const std::string& data);
    static std::string zipCompress(const std::string& data);
    static std::string zipDecompress(const std::string& data);
    static std::string base64Decode(const std::string& data);
    static std::string base64Encode(const std::string& data);
    //return content-type according mime (mime.cpp)
    //if not found, return "application/octet-stream"
    static std::string getContentType(const std::string& fileName);
};

} //http
} //gorilla
