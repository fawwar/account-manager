#pragma once
#include "./import/json-develop/json_tools.hpp"

using json = nlohmann::json;

class LdapConfig
{

public:
	LdapConfig();
 
  int ldap_port;
  
  std::string host_name;
  static LdapConfig& getInstance();
};
