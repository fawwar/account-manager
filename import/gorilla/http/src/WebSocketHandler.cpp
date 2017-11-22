#include "WebSocketHandler.h"
#include "HttpStatusCode.h"
#include "HttpUtil.h"
#include "HttpConnection.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include <websocketpp/error.hpp>
#include <chrono>

namespace gorilla 
{
namespace http
{

void WebSocketHandler::_handle(const HttpConnectionPtr& conn, const Callback& next)
{
    HttpRequestPtr req = conn->req;
    HttpResponsePtr res = conn->res;
    //here means passed cookie or auth check
    //and be the last part of WS handshake
    std::string requestText = req->method + " " + req->originalUrl + " HTTP/1.1\r\n" + //status line
                req->headers.toString() + //headers
                "\r\n"; //end marker
    ws_server::connection_ptr con = s.get_connection();
    con->set_write_handler([this, conn](websocketpp::connection_hdl hdl, char const* data, size_t len){
        
        if(conn->wsClosed)
        {
          std::cout << "ws write after close, " << conn.get() << std::endl;
          return websocketpp::lib::error_code();
        }
        
        std::string s(data, len);
        conn->sendBody2(s);
        return websocketpp::lib::error_code();
    });
    con->set_message_handler([this, conn](websocketpp::connection_hdl hdl, message_ptr msg){
        onMessage(conn, msg->get_payload());
    });
    con->set_open_handler([this, conn](websocketpp::connection_hdl hdl){
        onOpen(conn);
        conn->wsClosed = false;
        conn->syncBytes([this, conn](boost::system::error_code ec){
            conn->wsClosed = true;
            onClose(conn);
            conn->ws_connection.reset(); //used to release all capture in handlers
        }); //start reading
    });
    con->set_close_handler([this, conn](websocketpp::connection_hdl hdl){
        //if browser crashes, close handler will not be called
        //so onClose relies on read error event
        if(conn->wsClosed)
        {
          std::cout << "ws close event duplicate, " << conn.get() << std::endl;
          return;
        }
        conn->wsClosed = true;
        boost::system::error_code ec;
        conn->connection->socket().close(ec);
        std::cout << "ws close event, " << conn.get() << std::endl;
    });
    conn->ws_connection = con;
    con->start();
    std::stringstream requestStream(requestText);
    requestStream >> *con;
    //con->eof();
    //parse response
}
void WebSocketHandler::onOpen(const HttpConnectionPtr& conn)
{
    std::cout << "WebSocketHandler::onOpen" << std::endl;
}
void WebSocketHandler::onMessage(const HttpConnectionPtr& conn, const std::string& message)
{
    std::cout << "WebSocketHandler::onMessage, " << message.length() << std::endl;
    //sendText(conn, message); //test ping pong
}
void WebSocketHandler::onClose(const HttpConnectionPtr& conn)
{
    std::cout << "WebSocketHandler::onClose" << std::endl;
}
WebSocketHandler::WebSocketHandler()
{
    //clear all log output
    s.clear_access_channels(websocketpp::log::alevel::all);
}
int WebSocketHandler::sendText(const HttpConnectionPtr& conn, const std::string& message)
{
    conn->io_service_ptr->post([conn, message](){
      if(conn->wsClosed)
      {
        std::cout << "conn close, send abort, " << conn.get() << std::endl;
        return;
      }
      conn->ws_connection->send(message);
    });
    return conn->wsPending;
}
int WebSocketHandler::sendBinary(const HttpConnectionPtr& conn, const std::string& message)
{   
    conn->io_service_ptr->post([conn, message](){
      if(conn->wsClosed)
      {
        std::cout << "conn close, send abort, " << conn.get() << std::endl;
        return;
      }
      conn->ws_connection->send(message, websocketpp::frame::opcode::binary);
    });
    return conn->wsPending;
}
int WebSocketHandler::wait(const HttpConnectionPtr& conn, int maxBytes, int timeoutMS, bool *abortWait)
{
    if(timeoutMS < 0)
    {
      return conn->wsPending;
    }
    auto t1 = std::chrono::system_clock::now();
    while(conn->wsPending > maxBytes)
    {
      if(abortWait && *abortWait) return -1;
      if(timeoutMS > 0)
      {
        auto t2 = std::chrono::system_clock::now();
        std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
        if(ms.count() > timeoutMS) return -2;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return 0;
}
void WebSocketHandler::close(const HttpConnectionPtr& conn, const std::string& reason)
{
    conn->io_service_ptr->post([conn, reason](){
      if(conn->wsClosed)
      {
        std::cout << "conn close, send abort, " << conn.get() << std::endl;
        return;
      }
      conn->ws_connection->close(websocketpp::close::status::normal, reason);
    });
}

} //http
} //gorilla
