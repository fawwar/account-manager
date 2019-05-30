# C++ HTTP Framework

This framework is built on top of cppnetlib and provides the following features

  - independent of underlying implementation, ie. cppnetlib can be replaced easily
  - modular design using handlers
  - URL routing supports regex matching, url name id matching
  - query parameter parsing
  - built-in handlers
  - supports gzip, chunked transfer

### Usage

the most important classes are HttpServer, HttpRouter, HttpConnection, HttpRequest, HttpResponse

a very simple skeleton
```cpp
#include "http/HttpServer.h"
#include "http/HttpRouter.h"
#include "http/StaticFileMiddleware.h"
...
int main()
{
    auto baseRouter = std::make_shared<HttpRouter>();
    baseRouter->use("/public", std::make_shared<StaticFileHandler>("."))
    HttpServer httpServer;
    httpServer.setBaseHandler(baseRouter);
    httpServer.start();
    httpServer.join();
    return 0;
}
```

### HttpServer class
setup, start, stop 

```cpp
HttpServer* setPort(int port); //set port, default 80
HttpServer* setAddress(const std::string& address); //set listen address, default 0.0.0.0
HttpServer* setHttps(bool enableHttps); //set https on/off, default false
void start(); //start on other thread
void join(); //wait for http thread stop
void stop(); //stop the http server
```

### HttpConnection class
represents an Http connection. It is created by HttpServer and most methods are used internally.

Users will see HttpConnectionPtr (a shared_ptr typedef) from the handler call.

HttpConnection contains cppnetlib connection shared_ptr and the real connection is controlled by the lifetime of cppnetlib connection shared_ptr. (it has "shutdown socket" in its destructor), therefore users should pay attention to the lifetime of HttpConnection object.

```cpp
typedef std::shared_ptr<HttpConnection> ptr;
std::shared_ptr<HttpRequest> req;
std::shared_ptr<HttpResponse> res;
std::shared_ptr<boost::asio::io_service> io_service_ptr; //the io_service used by HttpServer
```

### HttpRouter class
HttpRouter is a kind of handler, and does url routing. 

Users can mount handlers on HttpRouter, therefore HttpRouter can be chained.

```cpp
typedef std::function<void(const HttpConnectionPtr&, const Callback&)> handler_t;
HttpRouter* use(handler_t handler); //no path provided, match ALL
HttpRouter* use(const IRequestHandlerPtr& handler); //no path provided, match ALL
HttpRouter* use(const std::string& path, handler_t handler);
HttpRouter* use(const std::string& path, const IRequestHandlerPtr& handler);
//support get,post,del,put. for other verbs, use 'use' and handle method inside
HttpRouter* get(const std::string& path, handler_t handler);
HttpRouter* get(const std::string& path, const IRequestHandlerPtr& handler);

HttpRouter* post(const std::string& path, handler_t handler);
HttpRouter* post(const std::string& path, const IRequestHandlerPtr& handler);

//delete is reserved keyword in C++, use del instead
HttpRouter* del(const std::string& path, handler_t handler);
HttpRouter* del(const std::string& path, const IRequestHandlerPtr& handler);

HttpRouter* put(const std::string& path, handler_t handler);
HttpRouter* put(const std::string& path, const IRequestHandlerPtr& handler);
```
use() will match the beginning path segments, and other function will only match whole path segments

   - use("/api", ...) will match "/api/hello", "/api/world", etc
   - get("/api", ...) will only match "/api", "/api/hello" will not get into the handler


### URL routing (HttpRouter)
   supports exact match, name id extraction, regex
```
"/api/test" matches /api/test
"/api/:name" matches /api/123, name=123
"/{name}/{name2}" matches /api/test, name=api, name2=test
"/[(\\d+)x(\\d+)]" matches /123x456 group1=123, group2=456
"/[(\\d+)x(\\d+)]:id" matches /123x456 group1=123, group2=456, id=123x456

regex must use whole segment:
"/[regex]/[regex]/{name}" is ok
"/head[regex]/[regex]tail" is NOT ok
similarly,
"/api/{name}" is ok
"/api/head{name}" is NOT ok
```

the name id extraction and regex will match the results to HttpRouteParameters which can be accessed through req->routeParams

### HttpRequest class
represents http request. HttpRequest will put http request url, start line, headers into structured format.


the member variables are all public to remove unnecessary getter/setter functions, but users should not modify the variables in ordinary cases.
```cpp
std::string version; //1.0, 1.1
std::string method; //methods are case sensitive
std::string originalUrl;//full path, not relative to handler base
std::string ip;//client ip
int port;//client port
std::string path; //path without querystring
std::string body; //BodyParserHandler will fill this variable
std::string contentType; //headers.getContentType(), without charset
HttpHeaders headers;
HttpQueryStrings queryStrings;
HttpRouteParameters routeParams;
//storage between request handlers
HttpDataManager httpDataManager;
//fetch data from httpDataManager,
//create=false, return nullptr if not exists
//create=true, return default object if not exists
//default create=true
template <typename T>
std::shared_ptr<T> getData(const std::string& name, bool create = true);
```
httpDataManager is for data exchange between handlers. 

For example, BasicAuthHandler will save auth data into httpDataManager, and other handlers can get auth data by req->getData<HttpBasicAuth>("BasicAuth")

HttpRequest provides a util function to convert the type of shared_ptr, see the following example

```cpp
baseRouter->get("/auth", [](const HttpConnectionPtr& conn, const IRequestHandler::Callback& next){
    HttpRequestPtr req = conn->req;
    HttpResponsePtr res = conn->res;
    auto basicAuth = req->getData<HttpBasicAuth>("BasicAuth", true);
    res->sendText(basicAuth->username + "," + basicAuth->password);
});
```

### HttpResponse class
represents http response. it contains response headers, status code, and several util functions
```cpp
HttpHeaders headers;//response header
HttpStatusCode statusCode;//default 200
void sendJson(const std::string& s); //application/json
//convert map key-value to json flat layout
void sendJson(const std::map<std::string, std::string>& kv);
//boost ptree
void sendJson(const boost::property_tree::ptree& pt);
void sendXml(const std::string& s);//application/xml
void sendXml(const boost::property_tree::ptree& pt);
void sendText(const std::string& s);//text/plain
void sendHtml(const std::string& s);//text/html
//send file by file path
void sendFile(const std::string& fileName); //decide mime type by filename
```
the above functions will call flush() after sending, but for the following functions, users MUST call flush() explicity.
```cpp
//for continuous sending, use callback version to wait finish and send more
void send(const std::string& s);//send once
void send(const std::string& s, std::function<void(boost::system::error_code ec)> callback);
```
flush is required because some filters are required to be notified the end of content, such as chunked transfer stream or gzipped stream
```
void flush();
void flush(std::function<void(boost::system::error_code ec)> callback);
```

### HttpHeaders class
parse http message headers into structured format.
store as std::map<std::string, std::vector<Header> > internally

get/set headers. if there are multiple header items with same field name, use appendHeader
```cpp
void setHeader(const std::string& name, const std::string& value);
void setHeader(const std::string& name, int value);
void setHeader(const std::string& header);//will parse colon:
void setHeader(const Header& header);
void appendHeader(const std::string& name, const std::string& value); 
void appendHeader(const std::string& name, int value);
void appendHeader(const std::string& header);//will parse colon:
void appendHeader(const Header& header);
void removeHeader(const std::string& name); //delete headers

//get
std::string get(const std::string& name) const;
const std::vector<Header>& getAll(const std::string& name) const;
bool contains(const std::string& name) const;
```
### HttpQueryStrings class
it will parse query string and do urldecode
  - a[]=1&a[]=2&a[]=3 => use get("a[]") to access
  - a=1&a=2&a=3 => use get("a")
  - a[color]=red => use get("a[color]")

accessed by req->queryStrings
```cpp
std::string get(const std::string& name) const;
const std::vector<std::string>& getAll(const std::string& name) const;
```

### HttpRouteParameters class
it will store url pattern match results
```cpp
std::string get(const std::string& name);//for :name or {name} matching
//regexp
//index is the regex position index (from 0), return the nth regex matched
std::string get(int index);
//index2 is the group index (starts from 1)
//return the matched segment if index2=0
std::string get(int index1, int index2);
```
examples
```cpp
//will match "/2016"
baseRouter->get("/:year", [](const HttpConnectionPtr& conn, const IRequestHandler::Callback& next){
    HttpRequestPtr req = conn->req;
    HttpResponsePtr res = conn->res;
    res->sendText(req->routeParams.get("year"));
});
//will match "/123x456", sendText sends "123.456"
//req->routeParams.get(0) = req->routeParams.get(0,0) = "123x456"
baseRouter->get("/[(\\d+)x(\\d+)]", [](const HttpConnectionPtr& conn, const IRequestHandler::Callback& next){
    HttpRequestPtr req = conn->req;
    HttpResponsePtr res = conn->res;
    res->sendText(
        req->routeParams.get(0, 1) + "." +
        req->routeParams.get(0, 2)
    );
});
//will match "/16/01/02"
//req->routeParams.get("year") = "16" => req->routeParams.get(0)
baseRouter->get("/[(\\d{2})]:year/[(\\d{2})]/[(\\d{2})]", [](const HttpConnectionPtr& conn, const  IRequestHandler::Callback& next){
    HttpRequestPtr req = conn->req;
    HttpResponsePtr res = conn->res;
    res->sendText(
        req->routeParams.get("year") + "." +
        req->routeParams.get(1) + "." +
        req->routeParams.get(2)
    );
});
```

### Chunked transfer encoding support
  - enable chunked transfer by setting header "Transfer-Encoding: chunked"   (res->headers.sendHeader("Trasnfer-Encoding", "chunked");
  - HttpResponse object will check the header to pipe into related filter

### Gzipped output support
  - enable gzip output by setting header "Content-Encoding: gzip"
  - not all client support gzipped content. User should check request header "Accept-Encoding"
  - HttpResponse object will check the header to pipe into related filter

### Handlers notes
  HttpRouter supports two kinds of handler input
  
  - class implements IRequestHandler
  - handler_t function

IRequestHandler should implement void _handle(const HttpConnectionPtr& conn, const Callback& next) function

it has a "next" callback and support deferred call. However, the real connection is controlled by the lifetime of connection object, users should be aware of the lifetime of connection object especially when using lanmbda capture.

### Notes
   - same handler can be mounted multiple times if the handler is stateless (no internal state)
   - HttpRouter is stateless
   - the order of handlers are important, for example, BasicAuthHandler must be registered before other handlers use the basic auth data
   
### TODO
  - file resume download