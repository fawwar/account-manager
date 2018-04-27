#include "Util.h"
#include <limits.h> //PATH_MAX
#ifdef _WIN32
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#else
#include <unistd.h> //for readlink
#endif
#include <mutex>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

static boost::uuids::random_generator genuuid;

#ifdef _WIN32
//only support ANSI 
std::string Util::getExeDir()
{
    char buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    std::string::size_type pos = std::string(buffer).find_last_of("\\/");
    return std::string(buffer).substr(0, pos);
}
void Util::setWorkingDir(const std::string& path)
{
    SetCurrentDirectory(path.c_str());
}
#else
std::string Util::getExeDir()
{
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    if (count > 0)
    {
        std::string exePath(result, count);
        boost::filesystem::path p(exePath);
        boost::filesystem::path dir = p.parent_path();
        return dir.generic_string();
    }
    return "";
}
void Util::setWorkingDir(const std::string& path)
{
    chdir(path.c_str());
}
#endif

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