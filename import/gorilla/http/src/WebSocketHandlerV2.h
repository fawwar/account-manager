#pragma once
#include "WebSocketHandler.h"

namespace gorilla 
{
namespace http
{

class MessageV2
{
    friend class WebSocketHandlerV2;
    /*
    message type
    0: reserved,
    1: user defined header, body
    
    */
    uint8_t type;//message type, 0 is reserved, do not use 0
    uint16_t seqNum;//do not use
    uint8_t control;//do not use    
    std::string header;//additional info, set by user
    std::string body;//payload
  public:    
    MessageV2();
    /*
      layout: in network byte order
      1 byte type, 2 byte seq, 1 byte control, 2 byte len, header, body
    */
    std::string toString() const;
};

class WebSocketHandlerV2: public WebSocketHandler
{
public:
    WebSocketHandlerV2();
    void sendMessageV2(const HttpConnectionPtr& conn, const MessageV2& message);
    void sendMessageV2(const HttpConnectionPtr& conn, const std::string& header, const std::string& body);
};

} //http
} //gorilla
