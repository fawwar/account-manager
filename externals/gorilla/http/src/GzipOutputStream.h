#pragma once
#include <vector>
#include <boost/iostreams/filtering_streambuf.hpp>
#include "IOutputStream.h"

namespace gorilla
{
namespace http
{
/**
    GzipOutputStream provides gzip output
*/
class GzipOutputStream: public IOutputStream
{
    std::vector<char> vout;
    boost::iostreams::filtering_streambuf<boost::iostreams::output> out;
public:
    GzipOutputStream();
    void write(const std::string& s, std::function<void(boost::system::error_code ec)> callback);
    void flush(std::function<void(boost::system::error_code ec)> callback);
};

} //http
} //gorilla
