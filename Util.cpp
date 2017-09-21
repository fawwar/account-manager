#include "Util.h"
#include <limits.h> //PATH_MAX
#include <unistd.h> //for readlink
#include <mutex>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

static boost::uuids::random_generator genuuid;

std::string Util::getExeDir()
{
  char result[ PATH_MAX ];
  ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
  if(count > 0)
  {
    std::string exePath(result, count);
    boost::filesystem::path p(exePath);
    boost::filesystem::path dir = p.parent_path();
    return dir.generic_string();
  }
  return "";
}

std::string Util::uuid()
{
  static std::mutex mtx; //static var is thread safe in c++11
  std::lock_guard<std::mutex> lk(mtx);
  return boost::lexical_cast<std::string>(genuuid());
}
Json::Value Util::toJsonValue(const std::string& s)
{
    if(s.empty()) return Json::nullValue;
    std::stringstream ss(s);
    Json::Value j;
    ss >> j;
    return j;
}