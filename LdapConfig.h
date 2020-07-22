#pragma once
#include "./import/json-develop/json_tools.hpp"

using json = nlohmann::json;

class LdapConfig
{

public:
	
  std::string address; 
  int port;  
  std::string host_name;
  std::string output_reply; // Json format
  static LdapConfig& getInstance();
  std::string Write(const std::string &str_ldap_config_info);
  bool IsUpdateInfoVaild(const std::string &str_ldap_config_in);

private: 
  LdapConfig();
  void Read();

};
