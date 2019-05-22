#include <boost/range/algorithm.hpp>
#include "HttpConnection.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpServer.h"

namespace gorilla 
{
namespace http
{

std::atomic_int HttpConnection::totalConnections(0);

HttpConnection::HttpConnection(HttpServer& httpServer):
    httpServer(httpServer)
{
    ++totalConnections;
    io_service_ptr = httpServer.getIoServicePtr();
    wsClosed = false;
    seqNum = 0;
    wsIsWriting = false;
	wsIsReading = false;
    wsPending = 0;
    totalReadBytes = 0;
    //std::cout << "HttpConnection::HttpConnection():" << this << std::endl;
}

HttpConnectionPtr HttpConnection::create(HttpServer& httpServer, const Lib::HttpConnection& c, const Lib::HttpRequest& req)
{
    HttpConnectionPtr conn(new HttpConnection(httpServer));
    conn->connection = c;
    conn->req = HttpRequest::create(conn, req);
    conn->res = HttpResponse::create(conn);
    conn->originReq = req;
    return conn;
}

HttpConnection::~HttpConnection()
{
    //std::cout << "HttpConnection::~HttpConnection():" << this << std::endl;
    --totalConnections;
}

//cppnetlib send when 'set_headers'
void HttpConnection::sendStatusCode(const HttpStatusCode& statusCode)
{
    connection->set_status((Lib::HttpStatusCode)static_cast<unsigned int>(statusCode));
}

void HttpConnection::sendHeaders(const HttpHeaders& headers)
{
    std::vector<boost::network::http::request_header_narrow> hs;
    for(auto&& kv: headers.getAll())
    {
       for(auto&& v: kv.second)
       {
           boost::network::http::request_header_narrow h{v.first, v.second};
           hs.push_back(h);
       }
    }
    connection->set_headers(hs);//send
}

void HttpConnection::write(const std::string& data, std::function<void(boost::system::error_code ec)> callback)
{
    connection->write(data, callback);
}

void HttpConnection::sendBody(const std::string& data)
{
    connection->write(data, [](boost::system::error_code ec) {
      if(ec)
      {
        std::cout << "sendBody " << ec << std::endl;
      }
    });
}

void HttpConnection::sendBody(const std::string& data, std::function<void(boost::system::error_code)> callback)
{
    connection->write(data, [callback](boost::system::error_code ec) {
        callback(ec);
    });
}

void HttpConnection::sendBody2(const std::string& data)
{
	if (wsClosed) return; //MUST	
    //only called in io service thread
	{
		bool isAdded = false;
		std::lock_guard<std::mutex> lk(mtxQueue);
		if (!data.empty())
		{
			if (wsQueue.size() >= 2)
			{
				if (wsQueue.back().size() + data.size() <= 64 * 1024)
				{
					wsQueue.back().append(data);
					wsPending += data.size();
					isAdded = true;
				}				
			}			
			if (!isAdded)
			{
				wsQueue.push(data);
				wsPending += data.size();
			}
		}
		
		if (wsQueue.empty()) return;		
		if (wsIsWriting) return;
		wsIsWriting = true;
	}    
	//io_service_ptr->post([this]() {
		HttpConnectionPtr conn = shared_from_this();
		try {
			std::string *toSent = NULL;
			int wQsize = 0;
			{
				std::lock_guard<std::mutex> lk(mtxQueue);
				toSent = &wsQueue.front();
				wQsize = wsQueue.size();
			}
			/*
			connection->write(toSent, [conn](boost::system::error_code ec) {
			if(ec)
			{
			std::cout << "sendBody2 failed, " << ec << std::endl;

			return;
			}
			conn->wsIsWriting = false;
			conn->io_service_ptr->post([conn](){
			conn->sendBody2("");
			});
			});
			*/
			//std::cout << this << " sending " << (*toSent).length() << ", " << wQsize << ", " << wsPending << std::endl;
			
			connection->write(*toSent, [this, conn](boost::system::error_code ec) {
			//boost::asio::async_write(connection->socket(), boost::asio::buffer(*toSent), [this, conn](boost::system::error_code ec, std::size_t length) {
				if (ec)
				{
					std::cout << "sendBody2 failed, " << ec << std::endl;
					wsClosed = true;
					wsIsWriting = false;
					connection->socket().close(ec);
					return;
				}
				//std::cout << this << " sending done ";
				{
					std::lock_guard<std::mutex> lk(mtxQueue);
					wsPending -= wsQueue.front().size();
					wsQueue.pop();
					//std::cout << wsQueue.size() << " / " << wsPending << std::endl;
					wsIsWriting = false;
					if (wsQueue.empty())
					{						
						return;
					}
				}				
				conn->io_service_ptr->post([conn]() {
					conn->sendBody2("");
				});
			});
		}
		catch (...)
		{
			std::cout << "sendBody2 got exception!!!" << std::endl;
		}
	//});
    
}

void HttpConnection::readBody(int len, std::string& data, std::function<void(boost::system::error_code ec)> callback)
{
    int bufLen = readBuffer.length();
    if(bufLen > 0)
    {
        int takeBytes = (bufLen >= len) ? len: bufLen;
        data.append(readBuffer, 0, takeBytes);
        readBuffer = readBuffer.substr(takeBytes);
        len -= takeBytes;
    }
    if(len == 0)
    {
        callback(boost::system::error_code(boost::system::errc::success, boost::system::generic_category()));
        return;
    }
    connection->read([this, len, &data, callback](Lib::HttpServer::connection::input_range input, boost::system::error_code ec, std::size_t bytes_transferred, Lib::HttpConnection connection){
       this->readBuffer.append(boost::begin(input), bytes_transferred);
       if(ec)
       {
           callback(ec);
           return;
       }       
       this->readBody(len, data, callback);
    });
}

void HttpConnection::readBody2(std::string& data, std::function<void(boost::system::error_code ec)> callback)
{
	//assert(!wsIsReading);
    try{	
		/*
		HttpConnectionPtr conn = shared_from_this();
		wsIsReading = true;
		connection->socket().async_read_some(boost::asio::buffer(&readBuffer[0], 64 * 1024), [this, &data, callback, conn](const boost::system::error_code& ec, std::size_t bytes_transferred) {
			wsIsReading = false;
			if (ec)
			{				
				callback(ec);				
				return;
			}
			if (bytes_transferred)
			{				
				data.append(&readBuffer[0], bytes_transferred);
			}
			else
			{
				std::cout << "Q0" << std::endl;
			}
			callback(ec);
		});
		*/
		///*
		
			
    connection->read([this, &data, callback](Lib::HttpServer::connection::input_range input, boost::system::error_code ec, std::size_t bytes_transferred, Lib::HttpConnection connection){
       if(ec)
       {
           callback(ec);
           return;
       }
       data.append(boost::begin(input), bytes_transferred);
       callback(ec);
    });
	//*/
    }
    catch(...)
    {
      boost::system::error_code ec = make_error_code(boost::system::errc::connection_aborted);
      std::cout << "readBody2 got exception!!!" << std::endl;      
      callback(ec);
    }
}
/*
void HttpConnection::read(std::function<void(const uint8_t* buffer, int len, boost::system::error_code ec)> callback)
{
    connection->read([this, callback](Lib::HttpServer::connection::input_range input, boost::system::error_code ec, std::size_t bytes_transferred, Lib::HttpConnection connection){
       readBuffer = std::string(boost::begin(input), bytes_transferred);
       callback((const uint8_t*)readBuffer.data(), readBuffer.length(), ec);
    });
}
*/
void HttpConnection::read(std::function<void(const std::string& data, boost::system::error_code ec)> callback)
{
    try
    {
      connection->read([this, callback](Lib::HttpServer::connection::input_range input, boost::system::error_code ec, std::size_t bytes_transferred, Lib::HttpConnection connection){
        totalReadBytes += bytes_transferred;
        if(bytes_transferred > 0)
        {
          readBuffer = std::string(boost::begin(input), bytes_transferred);
        }
        else
        {
          readBuffer.clear();
        }
        callback(readBuffer , ec);
      });
    }
    catch(std::exception& e) //catch except in read connection->read function
    {
      //std::cout << "HttpConnection::read " << e.what() << std::endl;
      callback("", boost::system::errc::make_error_code(boost::system::errc::io_error));
    }
}

uint16_t HttpConnection::nextSeqNum()
{
    std::lock_guard<std::mutex> guard(mtxSeqNum);
    ++seqNum;
    return seqNum;
}

int HttpConnection::getTotalConnections()
{
    return totalConnections;
}

void HttpConnection::syncBytes(std::function<void(boost::system::error_code ec)> callback)
{
    wsBuffer.clear();
    readBody2(wsBuffer, [this, callback](boost::system::error_code ec){
        if(ec) 
        {
            std::cout << "read ws error! " << ec << std::endl;
            callback(ec);
            return;
        }
        if(wsClosed)
        {
            puts("ws closed flag!");
            callback(boost::system::error_code());
            return;
        }
		if (wsBuffer.length())
		{			
			ws_connection->read_some(wsBuffer.c_str(), wsBuffer.length());
		}        
        syncBytes(callback);
    });
}

int64_t HttpConnection::getTotalReadBytes()
{
    return totalReadBytes;
}

void HttpConnection::dropBytes(int64_t count, std::function<void(boost::system::error_code)> callback)
{
    //std::cout << "drop =" <<count <<",now=" << totalReadBytes << std::endl;
    if(count <= totalReadBytes)
    {
        //std::cout << "drop done" << std::endl;
        boost::system::error_code ec;
        callback(ec);
        return;
    }
    this->read([this, count, callback](const std::string& data, boost::system::error_code ec){
       if(ec || data.length() == 0)
       {
           callback(ec);
           return;
       }       
       this->dropBytes(count, callback);
    });
}

} //http
} //gorilla
