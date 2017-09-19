#pragma once
#include <http/IRequestHandler.h>
#include "LegacyHttp.h"

using namespace gorilla::http;

class AdapterHandler: public IRequestHandler
{
protected:
    WorkQueue& queue;
    void _handle(const HttpConnectionPtr& conn, const Callback& next);    
public:
    AdapterHandler(WorkQueue& queue);
};

