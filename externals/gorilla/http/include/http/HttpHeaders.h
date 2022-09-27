#pragma once
#include <vector>
#include <string>
#include <map>
#include "Lib.h"
#include <json/json.h>

namespace gorilla
{
namespace http
{
/**
    parse http message headers to make them accessible
    rfc7230: Each header field consists of a case-insensitive field name 
    followed by a colon (":"), optional leading whitespace, the field value, 
    and optional trailing whitespace.

    note: it will store headers with key value map store, 
    and use std::vector to keep order if there are multiple headers with same field name
    eg. Set-Cookie requires preserving order
    
    SHOULD use methods to access header
*/
class HttpHeaders
{
public:
  typedef std::pair<std::string, std::string> Header;

  void setHeaders(const std::vector<Lib::HttpRequestHeader>& headers);//cppnetlib
  void setHeaders(const std::vector<std::string>& headers);//will parse colon:
  void setHeaders(const std::string& headers); //: and newline
  void setHeaders(const Json::Value& j); //object kv

  void setHeader(const std::string& name, const Json::Value& value);  
  /*
  void setHeader(const std::string& name, const std::string& value);
  void setHeader(const std::string& name, int value);
  */
  void setHeader(const std::string& header);//will parse colon:
  void setHeader(const Header& header);
  
  void appendHeader(const std::string& name, const std::string& value);
  void appendHeader(const std::string& name, int value);
  void appendHeader(const std::string& header);//will parse colon:
  void appendHeader(const Header& header);
  
  void removeHeader(const std::string& name);
  
  //return the first one or empty string
  std::string get(const std::string& name) const;

  const std::vector<Header>& getAll(const std::string& name) const;
  const std::map<std::string, std::vector<Header> >& getAll() const;
  
  bool contains(const std::string& name) const;
  
  //util functions
  //set Date: conforms with spec, eg. "Date:Tue, 10 Jan 2017 08:02:34 GMT"
  void setDateNow();
  
  //parse Content-type,
  //"Content-Type: text/html; charset=utf-8" will return "text/html"
  std::string getContentType() const;
  
  void print() const; //for debug use
  
  Json::Value toJson() const;

  std::string toString() const;
  //check if header field contains headers which "content-length" should not be used together
  bool noContentLength(); 
  
  std::string getBoundary() const;
  
  //clear all data
  void clear();
private:
  std::map<std::string, std::vector<Header> > headers;
};

} //http
} //gorilla
