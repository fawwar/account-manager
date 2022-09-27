#pragma once

namespace gorilla 
{
namespace http
{

enum class HttpStatusCode
{
  OK = 200,
  PARTIAL_CONTENT = 206,
  MOVE_PERMANENTLY = 301,
  NOT_MODIFIED = 304,
  BAD_REQUEST = 400,
  UNAUTHORIZED = 401,
  FORBIDDEN = 403,
  NOT_FOUND = 404,    
  REQUEST_TIMEOUT = 408,
  RANGE_NOT_SATISFIABLE = 416,
  INTERNAL_SERVER_ERROR = 500,
  NOT_IMPLEMENT = 501
};

} //http
} //gorilla
