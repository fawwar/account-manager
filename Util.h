#pragma once
#include <string>
#include <json/json.h>

class Util
{
public:
  static Json::Value toJsonValue(const std::string& s);  
  static std::string getExeDir();
  static std::string uuid();
};