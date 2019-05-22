#pragma once
#include <stack>
#include <memory>
#include <functional>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/property_tree/ptree.hpp>
#include <json/json.h>
#include "ptr.h"
#include "HttpStatusCode.h"
#include "HttpHeaders.h"

namespace gorilla 
{
namespace http
{
/**
    HttpResponse stores headers and status code
    provides send util functions
*/
class HttpResponse: public std::enable_shared_from_this<HttpResponse>
{
    HttpResponse();
    bool headerSet; //set true when first send
    IOutputStreamPtr output;
    FileReaderPtr fileReader;
public:
    typedef std::function<void(HttpResponse*)> FinishCallback;
    HttpHeaders headers;//response header
    HttpStatusCode statusCode;//default 200
    HttpConnectionWeakPtr connection; //connection is weakptr
    std::stack<FinishCallback> finishCallbacks;

    static HttpResponsePtr create(const HttpConnectionPtr& connection);
    ~HttpResponse();
    
    bool isFresh() const; //if no other handler have sent
    
    HttpResponse* setStatusCode(const HttpStatusCode& statusCode);
    HttpResponse* setStatusCode(int statusCode);
    
    //send util functions, set content type and content length
    void sendJson(const std::string& s); //application/json
    //convert map key-value to json flat layout
    void sendJson(const std::map<std::string, std::string>& kv);
    //boost ptree
    void sendJson(const boost::property_tree::ptree& pt);
    //JsonCpp
    void sendJson(const Json::Value& j);
    void sendXml(const std::string& s);//application/xml
    void sendXml(const boost::property_tree::ptree& pt);
    void sendText(const std::string& s);//text/plain
    void sendHtml(const std::string& s);//text/html
    //send file by file path
    void sendFile(const std::string& fileName,  const std::string& rangeHeader = ""); //decide mime type by filename
    
    //TODO Range Request, rfc7233
    //void sendPartialFile(const std::string& fileName, uint64_t position);
    
    //send will not set content type/length, only send raw bytes

    void send(const std::string& s);//send once
    //for continuous sending, use callback version to wait finish and send more
    void send(const std::string& s, std::function<void(boost::system::error_code ec)> callback);
    
    //some filter requires end notification
    //SHOULD NOT send after flush
    void flush();
    void flush(std::function<void(boost::system::error_code ec)> callback);
    
    //add finish callback
    void addNotify(FinishCallback callback);
};

} //http
} //gorilla
