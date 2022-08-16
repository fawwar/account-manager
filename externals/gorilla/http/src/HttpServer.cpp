#include <boost/lexical_cast.hpp>
#include <boost/asio/ssl.hpp>
#include <iostream>
#include <csignal>
//#include <boost/stacktrace.hpp>
#include "HttpConnection.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpServer.h"
#include "DefaultRequestHandler.h"

namespace gorilla 
{
namespace http
{

void HttpServer::operator()(const Lib::HttpRequest& req, const Lib::HttpConnection& c)
{
    HttpConnectionPtr conn = HttpConnection::create(*this, c, req);
    std::string path = conn->req->path;
    baseHandler->handle(conn, [this, path, conn](std::exception_ptr e){
        HttpRequestPtr req = conn->req;
        HttpResponsePtr res = conn->res;
        if(e)
        {
            req->error = e;
            while(!req->paths.empty())
            {
                req->paths.pop();
            }
            req->paths.push(path); //reset the path stack
            errHandler->handle(conn, [conn](std::exception_ptr e){
                HttpRequestPtr req = conn->req;
                HttpResponsePtr res = conn->res;
                try
                {
                    std::rethrow_exception(req->error);
                }
                catch( std::exception& e)
                {
                    res->setStatusCode(HttpStatusCode::INTERNAL_SERVER_ERROR);
                    res->sendHtml(
                    std::string() + 
                    "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">" 
                    "<html><head><title>500 Internal Server Error</title></head><body>" 
                    "<h1>Internal Server Error</h1>"
                    "<p>" + req->method + " " +  
                    req->originalUrl + "</p>" + 
                    "<p>" + e.what() + "</p>" 
                    "</body></html>");
                }
            });
        }
        else
        {            
            //here means no handler process
            res->setStatusCode(HttpStatusCode::NOT_FOUND);
            res->sendHtml(
                std::string() +
                "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">" 
                "<html><head><title>404 Not Found</title></head><body>" 
                "<h1>Not Found</h1>"
                "<p>The requested URL " + req->originalUrl + " was not found on this server.</p>"
                "</body></html>");
                
        }
    });
}

HttpServer::HttpServer()
{
    port = 80;
    address = "0.0.0.0";
    enableHttps = false;
    cert_chain_file = "server.pem";
    private_key_file = "server.pem";
    tmp_dh_file = "dh2048.pem";
    key_password = "test";
    io_service_ptr = boost::make_shared<boost::asio::io_service>();
    io_work = boost::make_shared<boost::asio::io_service::work>(*io_service_ptr);
    baseHandler = std::make_shared<DefaultRequestHandler>();
    errHandler = std::make_shared<DefaultRequestHandler>();
}

HttpServer::~HttpServer()
{
  io_work.reset();
}

boost::shared_ptr<boost::asio::io_service>& HttpServer::getIoServicePtr()
{
    return io_service_ptr;
}

HttpServer* HttpServer::setBaseHandler(const IRequestHandlerPtr &baseHandler)
{
    this->baseHandler = baseHandler;
    return this;
}

HttpServer* HttpServer::setErrorHandler(const IRequestHandlerPtr &errHandler)
{
    this->errHandler = errHandler;
    return this;
}

HttpServer* HttpServer::setPort(int port)
{
    this->port = port;
    return this;
}

HttpServer* HttpServer::setAddress(const std::string& address)
{
    this->address = address;
    return this;
}

HttpServer* HttpServer::setHttps(bool enableHttps)
{
    this->enableHttps = enableHttps;
    return this;
}

HttpServer* HttpServer::set_cert_chain_file(const std::string& s)
{
    this->cert_chain_file = s;
	return this;
}

HttpServer* HttpServer::set_private_key_file(const std::string& s)
{
    this->private_key_file = s;
	return this;
}
HttpServer* HttpServer::set_tmp_dh_file(const std::string& s)
{
    this->tmp_dh_file = s;
	return this;
}
HttpServer* HttpServer::set_key_password(const std::string& s)
{
    this->key_password = s;
	return this;
}

void HttpServer::start()
{
    assert(!serverThread);
    serverThread = std::make_shared<boost::thread>([this](){
        std::cout << "begin http server" << std::endl;
        try{
            if(enableHttps)
            {
                boost::shared_ptr<boost::asio::ssl::context> ctx = boost::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);
                ctx->set_options(boost::asio::ssl::context::default_workarounds |
                    boost::asio::ssl::context::no_sslv2 |
                    boost::asio::ssl::context::single_dh_use);
                    /* Set keys */
                ctx->set_password_callback([this](std::size_t max_length, boost::asio::ssl::context_base::password_purpose purpose){
                    return key_password;
                });
                ctx->use_certificate_chain_file(cert_chain_file);
                ctx->use_private_key_file(private_key_file, boost::asio::ssl::context::pem);
                ctx->use_tmp_dh_file(tmp_dh_file);
                this->httpServer = std::make_shared<Lib::HttpServer>(Lib::HttpServer::options(*this)
                    .io_service(io_service_ptr)
                    .port(boost::lexical_cast<std::string>(port))
                    .address(address)
                    .reuse_address(true).context(ctx));
            }
            else
            {
                this->httpServer = std::make_shared<Lib::HttpServer>(Lib::HttpServer::options(*this)
                    .io_service(io_service_ptr)
                    .port(boost::lexical_cast<std::string>(port))
                    .address(address)
                    .reuse_address(true));
            }
            
            this->httpServer->run();
        }        
        catch(std::exception& e)
        {
            std::cout << e.what() << std::endl;
            //std::cout << boost::stacktrace::stacktrace() << std::endl;
            //std::raise(SIGABRT); //for debug only
            eptr = std::current_exception(); //capture
        }
        std::cout << "end http server" << std::endl;
        this->stop();
    });
}

void HttpServer::stop()
{
    assert(serverThread);
    if(httpServer)
    {
        httpServer->stop();
        io_work.reset();        
    }
}

void HttpServer::join()
{
    serverThread->join();
    serverThread.reset();
    if(eptr)
    {
      //forward the exception
      std::rethrow_exception(eptr);
    }
}

std::string HttpServer::getBuild()
{
    return std::string(__DATE__) + " " + std::string(__TIME__);
}

} //http
} //gorilla
