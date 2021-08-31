#pragma once
#include "IOutputStream.h"

namespace gorilla
{
namespace http
{
/**
    convert output to chunked encoding
*/
class ChunkedOutputStream: public IOutputStream
{
public:
    ChunkedOutputStream();
    void write(const std::string& s, std::function<void(boost::system::error_code ec)> callback);
    void flush(std::function<void(boost::system::error_code ec)> callback);
};

} //http
} //gorilla
