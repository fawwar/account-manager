#include <fstream>
#include <iostream>
#include <string>
#include <json/json.h>
#include "LdapConfig.h"

#include "gorilla/log/logger.h"
#include "gorilla/log/logger_config.h"


using namespace gorilla::log;

LdapConfig::LdapConfig()
{
	
  try 
  {
    std::ifstream ifs1("ldap_config.json");
    std::string text((std::istreambuf_iterator<char>(ifs1) ),
                       (std::istreambuf_iterator<char>()) );
    Json::Value root;
    Json::Reader reader;
    if(reader.parse(text, root))
    {
		
      if(root.isMember("host_name"))
      {
		  host_name = const_cast<char*>(root["host_name"].asString().c_str());
		  LOGGER_S(info)<<"LdapConfig.cpp host_name!!!!!!!!!" << host_name;
      }   
      if(root.isMember("ldap_port"))
      {
		  std::stringstream stringToint(root["ldap_port"].asString());
		  stringToint >> ldap_port;
		  LOGGER_S(info) << "LdapConfig.cpp ldap_port!!!!!!!!!" << ldap_port;
      }   
    }
  }
  catch(std::exception& e)
  {
    //file not exist or parse error
    std::cout << e.what() << std::endl;
  }
  
}

LdapConfig& LdapConfig::getInstance()
{
  static LdapConfig m;
  return m;
}
