#pragma once
#include "./import/json-develop/json_tools.hpp"

using json = nlohmann::json;

class Config
{
 // Config();  
public:
  Config();
  std::string permission;
  json::object_t DISABLE_PERMISSIONS;
  json::object_t DISABLE_ACCESSRIGHT;
  json::object_t DEFAULT_PERMISSIONS;
  json::object_t DEFAULT_ACCESSRIGHT;

  std::string host_name;
  std::string ldap_port;
  static Config& getInstance();
};
