#pragma once
#include "./import/json-develop/json_tools.hpp"

using json = nlohmann::json;

class LdapConfig
{

public:
	
  std::string address; 
  int port=0;
  //std::string port;  
  std::string host_name;
  int timeout;

  std::string Read();
  void ParseConfig();
  std::string permission;
  static LdapConfig& getInstance();
  std::string Write(const std::string &str_ldap_config_info);
  bool IsUpdateInfoVaild(const std::string &str_ldap_config_in);

  json::object_t DEFAULT_PERMISSIONS;
  json::object_t DEFAULT_ACCESSRIGHT;

private: 
  LdapConfig();
  //void Read();
  const std::string m_strConfigFilePath;

};
