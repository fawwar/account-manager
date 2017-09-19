#pragma once
#include <memory>

namespace gorilla 
{
namespace http
{

class HttpConnection;
class HttpRequest;
class HttpResponse;
class IRequestHandler;
class IOutputStream;
class FileReader;
class HttpDataItem;

typedef std::shared_ptr<HttpConnection> HttpConnectionPtr;
typedef std::weak_ptr<HttpConnection> HttpConnectionWeakPtr;
typedef std::shared_ptr<HttpRequest> HttpRequestPtr;
typedef std::shared_ptr<HttpResponse> HttpResponsePtr;
typedef std::shared_ptr<IRequestHandler> IRequestHandlerPtr;
typedef std::shared_ptr<IOutputStream> IOutputStreamPtr;
typedef std::shared_ptr<FileReader> FileReaderPtr;
typedef std::shared_ptr<HttpDataItem> HttpDataItemPtr;

} //http
} //gorilla
