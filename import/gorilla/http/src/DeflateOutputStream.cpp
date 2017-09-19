#include "DeflateOutputStream.h"
#include "HttpUtil.h"

namespace gorilla
{
namespace http
{

void DeflateOutputStream::write(const std::string& s, std::function<void(boost::system::error_code ec)> callback)
{
    nextOutput->write(HttpUtil::zipCompress(s), callback);
}

} //http
} //gorilla
