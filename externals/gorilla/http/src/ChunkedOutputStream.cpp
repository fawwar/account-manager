#include <sstream>
#include <iomanip>
#include "ChunkedOutputStream.h"

namespace gorilla
{
namespace http
{

ChunkedOutputStream::ChunkedOutputStream()
{
}

void ChunkedOutputStream::write(const std::string& s, std::function<void(boost::system::error_code ec)> callback)
{
    assert(s.length() > 0); //use flush to notify end;
    std::stringstream ss;
    ss << std::hex << s.length();
    std::string output = ss.str();
    output += "\r\n";
    output += s;
    output += "\r\n";
    nextOutput->write(output, callback);
}

void ChunkedOutputStream::flush(std::function<void(boost::system::error_code ec)> callback)
{
    nextOutput->write("0\r\n\r\n", callback);
}

} //http
} //gorilla
