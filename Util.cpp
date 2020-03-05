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
#include <sstream>
#include <iomanip>

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

//https://blog.rayhome.cc/2017/10/c-urlencode-urldecode/
static std::string charToHex(unsigned char c) 
{
  short i = c;
  std::stringstream s;
  s << "%" << std::setw(2) << std::setfill('0') << std::hex << i;
  return s.str();
}

static char hexToChar(const std::string &str) 
{
  short c = 0;
  if(!str.empty()) 
  {
    std::istringstream in(str);
    in >> std::hex >> c;
    if(in.fail()) 
    {
      throw std::runtime_error("stream decode failure");
    }
  }
  return static_cast<unsigned char>(c);
}
std::string Util::urlEncode(const std::string &toEncode) 
{
   std::ostringstream out;
   for(std::string::size_type i = 0, len = toEncode.length(); i < len; ++i) 
   {
     short t = toEncode.at(i);
     if(
       t == 45 ||          // hyphen
       (t >= 48 && t <= 57) ||       // 0-9
       (t >= 65 && t <= 90) ||       // A-Z
       t == 95 ||          // underscore
       (t >= 97 && t <= 122) ||  // a-z
       t == 126            // tilde
     )
     {
       out << toEncode.at(i);
     } 
     else
     {
       out << charToHex(toEncode.at(i));
     }
   }
   return out.str();
}

std::string Util::urlDecode(const std::string &toDecode) 
{
  std::ostringstream out;
  for(std::string::size_type i = 0, len = toDecode.length(); i < len; ++i) 
  {
    if(toDecode.at(i) == '%') 
    {
      std::string str(toDecode.substr(i+1, 2));
      out << hexToChar(str);
      i += 2;
    }
    else
    {
      out << toDecode.at(i);
    }
  }
  return out.str();
}
