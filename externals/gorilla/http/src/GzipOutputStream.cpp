#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <sstream>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include "GzipOutputStream.h"
#include "HttpUtil.h"

namespace gorilla
{
namespace http
{

GzipOutputStream::GzipOutputStream()
{
    out.push(boost::iostreams::gzip_compressor(boost::iostreams::gzip_params()));
    out.push(boost::iostreams::back_insert_device<std::vector<char>>(vout));
}
void GzipOutputStream::write(const std::string& s, std::function<void(boost::system::error_code ec)> callback)
{    
    //std::cout << "z:" << s.size() << std::endl;
    std::ostream outf(&out);
    outf.write(s.c_str(), s.size());
    std::string str(vout.begin(),vout.end());
    nextOutput->write(str, callback);
    vout.clear();
    //nextOutput->write(HttpUtil::gzipCompress(s), callback);
}
void GzipOutputStream::flush(std::function<void(boost::system::error_code ec)> callback)
{
    out.pop(); //force flush
    std::string str(vout.begin(),vout.end());
    nextOutput->write(str, [this, callback](boost::system::error_code ec){
        nextOutput->flush(callback);
    });
}

} //http
} //gorilla
