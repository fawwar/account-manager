#pragma once
#include "IRequestHandler.h"
#include "HttpConnection.h"

namespace gorilla 
{
namespace http
{
/**
    handle websocket
*/
typedef ws_server::message_ptr message_ptr;

class WebSocketHandler: public IRequestHandler
{
protected:
    ws_server s;
    void _handle(const HttpConnectionPtr& conn, const Callback& next);

    //overwrite and save conn, remember to release conn when onClose
    virtual void onOpen(const HttpConnectionPtr& conn);
    virtual void onMessage(const HttpConnectionPtr& conn, const std::string& message);
    virtual void onClose(const HttpConnectionPtr& conn);

    //helper function
    //text websocket message should be valid utf8
    //NOTICE: sendText, sendBinary return websocket queued bytes
    static int sendText(const HttpConnectionPtr& conn, const std::string& message);
    static int sendBinary(const HttpConnectionPtr& conn, const std::string& message);

    static void close(const HttpConnectionPtr& conn, const std::string& reason = "server close connection"); //send close command
public:
    WebSocketHandler();
};

} //http
} //gorilla
