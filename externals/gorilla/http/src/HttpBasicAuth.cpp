#include "HttpBasicAuth.h"

namespace gorilla
{
namespace http
{
bool HttpBasicAuth::isValid()
{
    return !username.empty();
}
std::string HttpBasicAuth::toString() const
{
    return username + ":" + password;
}

} //http
} //gorilla
