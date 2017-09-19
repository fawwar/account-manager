#include <boost/filesystem.hpp>
#include "StaticFileHandler.h"
#include "HttpConnection.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

namespace gorilla 
{
namespace http
{

namespace fs = boost::filesystem;

StaticFileHandler::StaticFileHandler(const std::string& baseDir)
{
    this->baseDir = baseDir;
}

StaticFileHandler::~StaticFileHandler()
{
}

void StaticFileHandler::_handle(const HttpConnectionPtr& conn, const Callback& next)
{
    HttpRequestPtr req = conn->req;
    HttpResponsePtr res = conn->res;
    if(req->method == "GET")
    {
        fs::path dir(baseDir);
        std::string top = req->paths.top();
        if(top.empty())
        {
          top = "/index.html";
        }
        else if(top[top.length() - 1] == '/')
        {
          top = top + "index.html";
        }
        fs::path dir2(top);
        fs::path fullPath = dir / dir2;        
        try
        {
            if(!fs::exists(fullPath))
            {
                next(nullptr);
                return;
            }
            else if(fs::is_directory(fs::status(fullPath)))
            {
                next(nullptr);
                return;
            }
            std::cout << "GET " + dir2.string() << std::endl;            
            //just for testing
            //res->headers.setHeader("Content-Encoding", "gzip");
            //res->headers.setHeader("Transfer-Encoding", "chunked");            
            res->sendFile(fullPath.string());
        }
        catch(std::exception &e)
        {
            next(std::current_exception());
        }        
    }
    else
    {
        next(nullptr);
    }
}

} //http
} //gorilla
