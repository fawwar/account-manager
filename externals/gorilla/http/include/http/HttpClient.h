#pragma once
#include <functional>
#include <boost/system/error_code.hpp>
#include <curl/curl.h>
#include "HttpStatusCode.h"
#include "HttpHeaders.h"
#include "HttpBasicAuth.h"

namespace gorilla
{
namespace http
{
/**
    HttpClient sync, async version
    to reuse: MUST call reset() first!
*/
class HttpClient
{
    CURL* handle;
    static size_t write_data(char *buffer, size_t size, size_t nmemb, void *userp);
    static size_t header_callback(char *buffer, size_t size, size_t nmemb, void *userp);

    void commonSetting(const std::string& url, const HttpHeaders& headers);
    bool commonPerform();
    struct curl_slist *mlist;
public:
    HttpClient();
    ~HttpClient();
    void reset(); //to resue, must call reset first! then set custom options
    HttpHeaders headers; //response headers
    HttpStatusCode statusCode;//200
    CURLcode retCode; //CURL_OK
    std::string strError; //""
    std::string body; //""
    
    //the following will remain same across different requests
    bool certCheck; //default false!!
    HttpBasicAuth basicAuth;
    /*
        this functions get all data at once, not stream
    */
    void get(const std::string& url, std::function<void(bool)> func);
    void get(const std::string& url, const HttpHeaders& headers, std::function<void(bool)> func);
    bool get(const std::string& url);
    bool get(const std::string& url, const HttpHeaders& headers);
    
    void post(const std::string& url, const std::string& postData, std::function<void(bool)> func);
    void post(const std::string& url, const HttpHeaders& headers, const std::string& postData, std::function<void(bool)> func);
    bool post(const std::string& url, const std::string& postData);
    bool post(const std::string& url, const HttpHeaders& headers, const std::string& postData);
    
    template<typename T>
    CURLcode setOption(CURLoption option, const T& v)
    {
      return curl_easy_setopt(handle, option, v);
    }
    
};

} //http
} //gorilla
