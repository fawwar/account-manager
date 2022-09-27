#include "IOutputStream.h"

namespace gorilla
{
namespace http
{

IOutputStream::IOutputStream()
{
}

void IOutputStream::write(const std::string& s, std::function<void(boost::system::error_code ec)> callback)
{
    if(nextOutput)
    {
        nextOutput->write(s, callback);
    }
    else
    {
        callback(boost::system::errc::make_error_code(boost::system::errc::success));
    }
}

void IOutputStream::flush(std::function<void(boost::system::error_code ec)> callback)
{
    if(nextOutput)
    {
        nextOutput->flush(callback);
    }
    else
    {
        callback(boost::system::errc::make_error_code(boost::system::errc::success));
    }
}

void IOutputStream::setNextOutput(const IOutputStreamPtr& nextOutput)
{
    this->nextOutput = nextOutput;
}

IOutputStream::~IOutputStream()
{
}

} //http
} //gorilla
