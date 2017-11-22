#pragma once
#include <stdexcept>
#include <boost/shared_ptr.hpp>
#include "Lib.h"
#include "ptr.h"

namespace gorilla
{
namespace http
{
/**
SHOULD call setBaseHandler to set url route handler
*/
class HttpServer
{
    std::shared_ptr<Lib::HttpServer> httpServer;
    std::shared_ptr<boost::thread> serverThread;
    boost::shared_ptr<boost::asio::io_service> io_service_ptr; //controlled by cppnetlib
    boost::shared_ptr<boost::asio::io_service::work> io_work;

    int port; //80
    std::string address;//"0.0.0.0"
    IRequestHandlerPtr baseHandler;//DefaultRequestHandler
    IRequestHandlerPtr errHandler;//DefaultRequestHandler
    
    bool enableHttps;//false
    std::string cert_chain_file;
    std::string private_key_file;
    std::string tmp_dh_file;
    std::string key_password;
    
    std::exception_ptr eptr;
public:

    //this is for cppnetlib handler, DO NOT use
    void operator()(const Lib::HttpRequest& req, const Lib::HttpConnection& c);
    
    HttpServer();
    ~HttpServer();
    
    boost::shared_ptr<boost::asio::io_service>& getIoServicePtr();
    
    //every request go through base handler
    HttpServer* setBaseHandler(const IRequestHandlerPtr& baseHandler);
    HttpServer* setErrorHandler(const IRequestHandlerPtr& errHandler);
    HttpServer* setPort(int port); //listen port, 80
    HttpServer* setAddress(const std::string& address); //listen address, "0.0.0.0"
    //https
    HttpServer* setHttps(bool enableHttps); //false
    HttpServer* set_cert_chain_file(const std::string& s);
    HttpServer* set_private_key_file(const std::string& s);
    HttpServer* set_tmp_dh_file(const std::string& s);
    HttpServer* set_key_password(const std::string& s);

    void start(); //start on other thread
    void join(); //wait for http thread stop
    void stop();
    
    std::string getBuild();
};

} //http
} //gorilla
