#include <iostream>
#include "ConnOutputStream.h"
#include "HttpConnection.h"

namespace gorilla
{
namespace http
{

ConnOutputStream::ConnOutputStream(const HttpConnectionPtr& connection)
{
    this->connection = connection;
}
ConnOutputStream::~ConnOutputStream()
{
    //std::cout << "~ConnOutputStream "  << std::endl;
}
void ConnOutputStream::write(const std::string& s, std::function<void(boost::system::error_code ec)> callback)
{
    HttpConnectionPtr conn = connection.lock();
    if(conn)
    {
        if(s.size() != 0)
        {
            conn->sendBody(s, callback);
        }
        else
        {
            callback(boost::system::errc::make_error_code(boost::system::errc::success));
        }
    }
    else
    {
        callback(boost::system::errc::make_error_code(boost::system::errc::operation_canceled));
    }
}

} //http
} //gorilla
