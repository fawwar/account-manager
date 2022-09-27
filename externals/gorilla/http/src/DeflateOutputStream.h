#pragma once
#include "IOutputStream.h"

namespace gorilla
{
namespace http
{
/**
    DeflateOutputStream provides zlib/deflate output
*/
class DeflateOutputStream: public IOutputStream
{
public:
    void write(const std::string& s, std::function<void(boost::system::error_code ec)> callback);
};

} //http
} //gorilla
