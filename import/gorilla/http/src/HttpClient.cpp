#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include "HttpClient.h"
#include "JobManager.h"

/**
TODO: check Expect behavior, since cppnetlib doesn't handle 100-continue;
*/

namespace gorilla
{
namespace http
{

size_t HttpClient::write_data(char *buffer, size_t size, size_t nmemb, void *userp)
{
    //TODO should monitor the size of body!
    HttpClient* pThis = (HttpClient*) userp;    
    size_t len = size * nmemb;
    pThis->body.append(buffer, len);
    return len;
}
size_t HttpClient::header_callback(char *buffer, size_t size, size_t nmemb, void *userp)
{
    HttpClient* pThis = (HttpClient*) userp;
    size_t len = size * nmemb;
    if(len == 0) return 0; //end
    try
    {
        std::string header = std::string(buffer, len);
        if(boost::algorithm::starts_with(header, "HTTP/1.1"))
        {
            if(header.length() >= 12)
            {
                pThis->statusCode = (HttpStatusCode) boost::lexical_cast<int>(header.substr(9,3));
            }
        }
        else
        {
            pThis->headers.appendHeader(header);
        }
    }
    catch(std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return len;
}

HttpClient::HttpClient()
{
     statusCode = HttpStatusCode::OK;
     handle = curl_easy_init();
     certCheck = false;
     retCode = CURLE_OK;
     mlist = NULL;
     reset();
}
HttpClient::~HttpClient()
{
     curl_easy_cleanup(handle);
}
void HttpClient::reset()
{
    curl_easy_reset(handle);//blank all previous options
    headers.clear();
    statusCode = HttpStatusCode::OK;
    retCode = CURLE_OK;
    strError.clear();
    body.clear();
    mlist = NULL;
}
void HttpClient::commonSetting(const std::string& url, const HttpHeaders& headers)
{
    //reset(); //clear all state, options
    curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(handle, CURLOPT_HEADER, 0L);
    curl_easy_setopt(handle, CURLOPT_HEADERDATA, this);
    curl_easy_setopt(handle, CURLOPT_HEADERFUNCTION, header_callback);
    curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, certCheck ? 1L : 0L);
    //set header first
    auto& h = headers.getAll();    
    for(auto const& it1: h)
    {
        for(auto& it2: it1.second)
        {
            mlist = curl_slist_append(mlist, (it2.first + ": " + it2.second).c_str());
        }
    }
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, mlist);
    //then set other options that might alter the header
    
    //curl_easy_setopt(easyhandle, CURLOPT_HTTPAUTH,  CURLAUTH_DIGEST|CURLAUTH_BASIC);
    //curl_easy_setopt(easyhandle, CURLOPT_USERPWD, "myname:thesecret");
    
    if(basicAuth.isValid())
    {
        //do basic auth
        curl_easy_setopt(handle, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
        curl_easy_setopt(handle, CURLOPT_USERPWD, basicAuth.toString().c_str());
    }    
    
    //curl_easy_setopt(handle, CURLOPT_VERBOSE, 1L);
}
bool HttpClient::commonPerform()
{
    this->retCode = curl_easy_perform(handle); //CURLE_OK    
    curl_slist_free_all(mlist); //inside has null check
    if(this->retCode == CURLE_OK) return true;
    strError = curl_easy_strerror(retCode);
    return false;
}
void HttpClient::get(const std::string& url, std::function<void(bool)> func)
{
    get(url, HttpHeaders(), func);
}
void HttpClient::get(const std::string& url, const HttpHeaders& headers, std::function<void(bool)> func)
{
    /*
    TODO: use curl multi socket
    */
    JobManager& jm = JobManager::GetInstance();
    jm.doJob([=](){
        bool b = this->get(url, headers);
        func(b);        
    });
}
bool HttpClient::get(const std::string& url)
{
    return get(url, HttpHeaders());
}
bool HttpClient::get(const std::string& url, const HttpHeaders& headers)
{
    commonSetting(url, headers);
    //curl_easy_setopt(handle, CURLOPT_HTTPGET, 1L); //should be optional
    return commonPerform();    
}

//POST
void HttpClient::post(const std::string& url, const std::string& postData, std::function<void(bool)> func)
{
    post(url, HttpHeaders(), postData, func);
}
void HttpClient::post(const std::string& url, const HttpHeaders& headers, const std::string& postData, std::function<void(bool)> func)
{
    /*
    TODO: use curl multi socket
    */
    JobManager& jm = JobManager::GetInstance();
    jm.doJob([=](){
        bool b = this->post(url, headers, postData);
        func(b);        
    });
}
bool HttpClient::post(const std::string& url, const std::string& postData)
{
    return post(url, HttpHeaders(), postData);
}
bool HttpClient::post(const std::string& url, const HttpHeaders& headers, const std::string& postData)
{
    commonSetting(url, headers);
    curl_easy_setopt(handle, CURLOPT_POSTFIELDS, postData.c_str()); //force POST
    return commonPerform();
}



} //http
} //gorilla
