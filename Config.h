#pragma once
#include "json-develop/json_tools.hpp"

using json = nlohmann::json;

class Config
{
  Config();  
public:

  std::string permission;
  std::string account_regexpr;
  std::string password_regexpr;

  json::object_t DISABLE_PERMISSIONS;
  json::object_t DISABLE_ACCESSRIGHT;
  json::object_t DEFAULT_PERMISSIONS;
  json::object_t DEFAULT_ACCESSRIGHT;

  
  
  static Config& getInstance();
};
