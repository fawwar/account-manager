#include <sstream>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "HttpBoostPtree.h"

namespace gorilla
{
namespace http
{

void HttpBoostPtree::readXml(const std::string& s)
{
    std::stringstream ss(s);
    boost::property_tree::read_xml(ss, pt);
}

void HttpBoostPtree::readJson(const std::string& s)
{
    std::stringstream ss(s);
    boost::property_tree::read_json(ss, pt);
}

}//http
}//gorilla
