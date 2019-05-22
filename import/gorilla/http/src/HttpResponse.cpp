#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "HttpResponse.h"
#include "HttpUtil.h"
#include "HttpServer.h"
#include "ChunkedOutputStream.h"
#include "GzipOutputStream.h"
#include "ConnOutputStream.h"
#include "IdentityOutputStream.h"
#include "DeflateOutputStream.h"
#include "IOutputStream.h"
#include "HttpConnection.h"
#include "FileReader.h"

namespace gorilla
{
namespace http
{

HttpResponse::HttpResponse():
    statusCode(HttpStatusCode::OK),
    headerSet(false)
{
    headers.setHeader("Content-type", "text/plain; charset=utf-8");
    headers.setHeader("Connection", "close");
    headers.setHeader("Server", "Gorilla HTTP");
}

HttpResponsePtr HttpResponse::create(const HttpConnectionPtr& connection)
{
    HttpResponsePtr sp(new HttpResponse());
    sp->connection = connection;
    return sp;
}

HttpResponse::~HttpResponse()
{
    //std::cout << "~HttpResponse()" << std::endl;
    //do notify!
    while(!finishCallbacks.empty())
    {
        FinishCallback& cb = finishCallbacks.top();
        cb(this);
        finishCallbacks.pop();
    }
}

bool HttpResponse::isFresh() const
{
    return headerSet;
}

HttpResponse* HttpResponse::setStatusCode(const HttpStatusCode& statusCode)
{
    this->statusCode = statusCode;
    return this;
}

HttpResponse* HttpResponse::setStatusCode(int statusCode)
{
    return setStatusCode((HttpStatusCode)statusCode);
}

/*
    deflate:
    std::string test = HttpUtil::zipCompress(s);
    headers.setHeader("Content-Length", test.length());
    headers.setHeader("Content-Encoding","deflate");
    gzip is similar
    verified with curl/wireshark
*/

void HttpResponse::sendJson(const std::string& s)
{
    headers.setHeader("Content-type", "application/json; charset=utf-8");    
    headers.setHeader("Content-Length", s.length());
    send(s, [this](boost::system::error_code ec){
        this->flush();
    });
}

void HttpResponse::sendJson(const std::map<std::string, std::string>& kv)
{
    using boost::property_tree::ptree;
    using boost::property_tree::basic_ptree;
    using boost::property_tree::write_json;
    ptree pt;
    for(auto&& it: kv)
    {
       pt.put<std::string>(it.first, it.second);
    }
    std::stringstream ss;
    write_json(ss, pt);
    sendJson(ss.str());
}

void HttpResponse::sendJson(const boost::property_tree::ptree& pt)
{
    std::stringstream ss;
    boost::property_tree::write_json(ss, pt);
    sendJson(ss.str());
}

void HttpResponse::sendJson(const Json::Value& j)
{
    sendJson(j.toStyledString());
}

void HttpResponse::sendXml(const std::string& s)
{
    headers.setHeader("Content-type", "application/xml");
    headers.setHeader("Content-Length", s.length());
    send(s, [this](boost::system::error_code ec){
        this->flush();
    });
}

void HttpResponse::sendXml(const boost::property_tree::ptree& pt)
{
    std::stringstream ss;
    boost::property_tree::write_xml(ss, pt);
    sendXml(ss.str());
}

void HttpResponse::sendText(const std::string& s)
{
    headers.setHeader("Content-type", "text/plain; charset=utf-8");
    headers.setHeader("Content-Length", s.length());
    send(s, [this](boost::system::error_code ec){
        this->flush();
    });
}

void HttpResponse::sendHtml(const std::string& s)
{
    headers.setHeader("Content-type", "text/html; charset=utf-8");
    headers.setHeader("Content-Length", s.length());
    send(s, [this](boost::system::error_code ec){
        this->flush();
    });
}

void HttpResponse::sendFile(const std::string& fileName, const std::string& rangeHeader)
{
    HttpConnectionPtr connection = this->connection.lock();
    boost::filesystem::path fullPath(fileName);
    //here IO api is sync(filesize, stat, open)
    auto fsize = boost::filesystem::file_size(fullPath);
    std::vector<std::string> s1;
    if(!rangeHeader.empty())
    {
      boost::split(s1, rangeHeader, boost::is_any_of("-"));
    }
    size_t beginPos = 0;
    size_t endPos = fsize;
    if(s1.size() == 1)
    {
      if(!s1[0].empty())
      {
        beginPos = boost::lexical_cast<size_t>(s1[0]);
      }
    }
    else if(s1.size() == 2)
    {
      if(!s1[0].empty())
      {
        beginPos = boost::lexical_cast<size_t>(s1[0]);
      }
      if(!s1[1].empty())
      {
        endPos = boost::lexical_cast<size_t>(s1[1]) + 1;
      }
    }
    else
    {
      //wrong or 0
    }
    if(beginPos > endPos || endPos > fsize) //wrong range
    {
      this->setStatusCode(HttpStatusCode::RANGE_NOT_SATISFIABLE);
      std::stringstream ss;
      ss << "*/" << fsize;
      headers.setHeader("Content-Range", ss.str());
      send(""); 
      return;
    }
    else if(s1.size() > 0)
    {
      std::stringstream ss;      
      ss << "bytes ";
      ss << beginPos << "-" << (endPos - 1) << "/" << fsize;
      headers.setHeader("Content-Range", ss.str());
    }
    headers.setHeader("Content-Length", endPos - beginPos);
    headers.setHeader("Content-type", HttpUtil::getContentType(fileName));
    fileReader = std::make_shared<FileReader>();
    fileReader->setFileName(fileName);
    if(s1.size() > 0)
    {
      fileReader->setPosition1(beginPos);
      fileReader->setPosition2(endPos);
    }
    //connection must be captured!
    fileReader->on("data", [connection, this](const char* buffer, int len){
        this->send(std::string(buffer, len), [connection, this](boost::system::error_code ec){
            if(!ec)
            {
               this->fileReader->read();
            }
            else
            {
               //std::cout << ec.message() << std::endl;
               this->flush();
               this->fileReader.reset();
            }
        });
    });
    fileReader->on("close", [connection, this](const char* buffer, int len){
        this->flush();
        this->fileReader.reset(); //must do here to release callback capture
    });
    fileReader->read(); //fileReader is captured in event callback registration
}

void HttpResponse::send(const std::string& s)
{
    send(s, [](boost::system::error_code ec){});
}
void HttpResponse::send(const std::string& s, std::function<void(boost::system::error_code ec)> callback)
{
    try{
      HttpConnectionPtr connection = this->connection.lock();
      if(!headerSet)
      {
        //check header
        IOutputStreamPtr curr = std::make_shared<IdentityOutputStream>();
        this->output = curr;
        if(headers.noContentLength())
        {
            headers.removeHeader("Content-Length");
        }
        if(headers.get("Content-Encoding") == "gzip")
        {
            auto p = std::make_shared<GzipOutputStream>();
            curr->setNextOutput(p);
            curr = p;
        }
        else
        {
            //do not support encoding other than gzip
            headers.removeHeader("Content-Encoding");
        }
        /*
        else if(headers.get("Content-Encoding") == "deflate")
        {
            auto p = std::make_shared<ZipOutputStream>();
            curr->setNextOutput(p);
            curr = p;
            
        }
        */
        if(headers.get("Transfer-Encoding") == "chunked")
        {
            auto p = std::make_shared<ChunkedOutputStream>();
            curr->setNextOutput(p);
            curr = p;
        }
        curr->setNextOutput(std::make_shared<ConnOutputStream>(connection));
        //send status, header        
        headers.setDateNow();
        connection->sendStatusCode(statusCode);
        connection->sendHeaders(headers);
        
        headerSet = true;
      }
      output->write(s, [connection, callback](boost::system::error_code ec){
        callback(ec);
      });
    }
    catch(std::exception& e)
    {
      //set status, set header will throw
      callback(boost::system::errc::make_error_code(boost::system::errc::operation_canceled));
    }
}
void HttpResponse::flush()
{
    output->flush([](boost::system::error_code ec){
    });
}
void HttpResponse::flush(std::function<void(boost::system::error_code ec)> callback)
{
    HttpConnectionPtr connection = this->connection.lock();
    output->flush([connection, callback](boost::system::error_code ec){
        callback(ec);
    });
}
void HttpResponse::addNotify(FinishCallback callback)
{
    finishCallbacks.push(callback);
}


} //http
} //gorilla
