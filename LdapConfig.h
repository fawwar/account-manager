#pragma once
#ifdef LDAP_OPTION
#include "./import/json-develop/json_tools.hpp"

using json = nlohmann::json;

class LdapConfig
{

public:
	
  std::string address; 
  int port;  
  std::string host_name;
  static LdapConfig& getInstance();
  std::string Write(const std::string &str_ldap_config_info);
  bool IsUpdateInfoVaild(const std::string &str_ldap_config_in);

private: 
  LdapConfig();
  void Read();

};
#endif
