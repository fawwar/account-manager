#pragma once
#include "./import/json-develop/json_tools.hpp"

using json = nlohmann::json;

class Config
{
  Config();  
public:
  std::string permission;
  json::object_t DISABLE_PERMISSIONS;
  json::object_t DISABLE_ACCESSRIGHT;
  json::object_t DEFAULT_PERMISSIONS;
  json::object_t DEFAULT_ACCESSRIGHT;
  
  static Config& getInstance();
};
