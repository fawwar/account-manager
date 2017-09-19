#pragma once
#include <string>
#include <boost/property_tree/ptree.hpp>
#include "HttpDataItem.h"

namespace gorilla
{
namespace http
{
/**
    for body parser result
*/
class HttpBoostPtree: public HttpDataItem
{
public:
    boost::property_tree::ptree pt;
    void readXml(const std::string& s);
    void readJson(const std::string& s);
};

} //http
} //gorilla
