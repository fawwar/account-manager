#include "WebSocketHandlerV2.h"
#include "HttpStatusCode.h"
#include "HttpUtil.h"
#include "HttpConnection.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include <websocketpp/error.hpp>

namespace gorilla 
{
namespace http
{

MessageV2::MessageV2()
{
    seqNum = 0;
    control = 0;
    type = 1;
}
std::string MessageV2::toString() const
{
    int len = 1 + 2 + 1 + 2 + header.length();
    uint16_t headerLen = (uint16_t) header.length();
    std::string data( len + body.length() , 0);
    data[0] = type;
    data[1] = (seqNum >> 8) & 0xFF;
    data[2] = seqNum & 0xFF;
    data[3] = control;
    data[4] = (headerLen >> 8) & 0xFF;
    data[5] = headerLen & 0xFF;
    data.replace(6, headerLen, header);
    data.replace(len, body.length(), body);
    return data;
}

WebSocketHandlerV2::WebSocketHandlerV2()
{
}

void WebSocketHandlerV2::sendMessageV2(const HttpConnectionPtr& conn, const MessageV2& message)
{
    uint16_t seqNum = conn->nextSeqNum();
    std::string data = message.toString();
    data[1] = (seqNum >> 8) & 0xFF;
    data[2] = seqNum & 0xFF;
    sendBinary(conn, data);   
}
void WebSocketHandlerV2::sendMessageV2(const HttpConnectionPtr& conn, const std::string& header, const std::string& body)
{
    MessageV2 message;
    message.type = 1;
    message.header = header;
    message.body = body;
    sendMessageV2(conn, message);
}

} //http
} //gorilla
