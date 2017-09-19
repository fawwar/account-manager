#pragma once
#include <memory>
#include <boost/system/error_code.hpp>
#include "ptr.h"

namespace gorilla
{
namespace http
{
/**
    IOutputStream provides an interface for async write
*/
class IOutputStream
{
public:
    virtual void write(const std::string& s, std::function<void(boost::system::error_code ec)> callback);
    virtual void flush(std::function<void(boost::system::error_code ec)> callback);
    void setNextOutput(const IOutputStreamPtr& nextOutput);
    IOutputStream();
    virtual ~IOutputStream();
protected:
    IOutputStreamPtr nextOutput;
};

} //http
} //gorilla
