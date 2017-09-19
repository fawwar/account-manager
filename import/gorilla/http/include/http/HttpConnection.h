#pragma once
#include <memory>
#include <stack>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <websocketpp/config/core.hpp>
#include <websocketpp/server.hpp>
#include "ptr.h"
#include "Lib.h"
#include "HttpStatusCode.h"
#include "HttpHeaders.h"

namespace gorilla 
{
namespace http
{
//put websocketpp connection ptr in HttpConnection
//easily for lifetime management and mapping
typedef websocketpp::server<websocketpp::config::core> ws_server;

class HttpServer;
class HttpRequest;
class HttpResponse;
class ConnOutputStream;
/**
    HttpConnection contains request, response, cppnetlib connection
*/
class HttpConnection: public std::enable_shared_from_this<HttpConnection>
{
    friend class HttpRequest;
    friend class HttpResponse;
    friend class ConnOutputStream;
    friend class WebSocketHandler;
    friend class WebSocketHandlerV2;
    
    static std::atomic_int totalConnections; //for stats
    
    int64_t totalReadBytes;

    std::atomic_bool wsClosed;//used for sync onclose event of websocketpp
    std::string wsBuffer; //for websocketpp read buffer
    ws_server::connection_ptr ws_connection; //websocketpp, default null ptr
    std::queue<std::string> wsQueue;
    int wsPending; //pending bytes        
    bool wsIsWriting;    
    
    
    uint16_t seqNum; //for websocket handler v2
    std::mutex mtxSeqNum;
    uint16_t nextSeqNum();

    std::string readBuffer;
    HttpConnection(HttpServer& httpServer);
    
    //used by HttpResponse
    void sendStatusCode(const HttpStatusCode& statusCode);
    void sendHeaders(const HttpHeaders& headers);
    
    //sendBody,sendFile are async, used by HttpResponse
    void sendBody(const std::string& data);
    
    void sendBody(const std::string& data, std::function<void(boost::system::error_code ec)> callback);
    void readBody(int len, std::string& data, std::function<void(boost::system::error_code ec)> callback);
    void readBody2(std::string& data, std::function<void(boost::system::error_code ec)> callback);
    void syncBytes(std::function<void(boost::system::error_code ec)> callback); //for websocketpp, callback is called when read error
    
    //sendBody2 only used by websocket handler
    void sendBody2(const std::string& data);
    
    
public:

    Lib::HttpConnection connection;//cppnetlib
    HttpRequestPtr req;
    HttpResponsePtr res;
    Lib::HttpRequest originReq; //cppnet req
    boost::shared_ptr<boost::asio::io_service> io_service_ptr;
    HttpServer& httpServer;
    static HttpConnectionPtr create(HttpServer& httpServer, const Lib::HttpConnection& c, const Lib::HttpRequest& req);
    ~HttpConnection();
    static int getTotalConnections();
    //advanced read function, to read from body
    void read(std::function<void(const std::string& data, boost::system::error_code ec)> callback);
    void write(const std::string& data, std::function<void(boost::system::error_code ec)> callback);
    void dropBytes(int64_t count, std::function<void(boost::system::error_code ec)> callback);
    int64_t getTotalReadBytes();
};

} //http
} //gorilla
