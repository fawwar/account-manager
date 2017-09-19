#include <http/HttpConnection.h>
#include <http/HttpRequest.h>
#include <http/HttpResponse.h>
#include "AdapterHandler.h"


void AdapterHandler::_handle(const HttpConnectionPtr& conn, const Callback& next)
{
   //extract raw conn ptr, req info to RequestData
   //std::cout << "process " << conn->req->originalUrl << std::endl;
   queue.put(std::make_shared<RequestData>(conn->originReq, conn->connection));
}


AdapterHandler::AdapterHandler(WorkQueue& queue):
    queue(queue)
{
}

