#pragma once
#include <string>
#include <json/json.h>

class Util
{
public:
  static Json::Value toJsonValue(const std::string& s);  
  static std::string getExeDir();
  static void setWorkingDir(const std::string& path);
  static std::string uuid();
  static std::string urlEncode(const std::string &toEncode);
  static std::string urlDecode(const std::string &toDecode);
};