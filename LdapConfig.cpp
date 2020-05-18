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
  host_name = "192.168.0.248";
  port = 389;
	
  try 
  {
    Read();
  }
  catch(std::exception& e)
  {
    //file not exist or parse error
    std::cout << e.what() << std::endl;
  }
  
}

void LdapConfig::Read()  // read to json
{
	std::ifstream ifs1("ldap_config.json");
	std::string text((std::istreambuf_iterator<char>(ifs1)),
		(std::istreambuf_iterator<char>()));
	Json::Value root;
	Json::Reader reader;
	if (reader.parse(text, root))
	{

		if (root.isMember("host_name"))
		{
			host_name = root["host_name"].asString().c_str();
			//LOGGER_S(info) << "LdapConfig.cpp host_name!!!!!!!!!" << host_name;
		}
		if (root.isMember("ldap_port"))
		{
			port = root["ldap_port"].asInt();
			//LOGGER_S(info) << "LdapConfig.cpp ldap_port!!!!!!!!!" << port;
		}
		if (root.isMember("address"))
		{
			address = root["address"].asString().c_str();
			//LOGGER_S(info) << "LdapConfig.cpp address!!!!!!!!!" << address;
		}
	}
}


std::string LdapConfig::Write(const std::string &str_ldap_config_info)  // write to json
{
	Json::Reader reader;
	Json::Value root;
	reader.parse(str_ldap_config_info, root);
	std::ofstream op_file;   //open json file 
	op_file.open("ldap_config.json");			
	LdapConfig &ldapConfig = LdapConfig::getInstance();

	try {
		if (reader.parse(str_ldap_config_info, root))
		{
			if (root.isMember("host_name"))
			{
				ldapConfig.host_name = root["host_name"].asString().c_str();
				root["host_name"] = ldapConfig.host_name;
				//LOGGER_S(info)<<"Error AccountManager::UpdateLdapConfig host_name!!!!!!!!!" << ldapconfig.host_name;
			}
			if (root.isMember("ldap_port"))
			{
				ldapConfig.port = root["ldap_port"].asInt();
				root["ldap_port"] = ldapConfig.port;
				//LOGGER_S(info) << "Error AccountManager::UpdateLdapConfig ldap_port!!!!!!!!!" << ldapconfig.ldap_port;
			}
                        if (root.isMember("address"))
			{
				ldapConfig.address = root["address"].asString().c_str();
				root["address"] = ldapConfig.address;
				//LOGGER_S(info) << "Error AccountManager::UpdateLdapConfig ldap_port!!!!!!!!!" << ldapconfig.ldap_port;
			}
		}
	}
	catch (char const* error) {
		LOGGER_S(debug) << "Fail to Update LdapConfig to Json file " << reader.getFormattedErrorMessages();
	}

	Json::StyledWriter styledWriter;
	op_file << styledWriter.write(root);
	op_file.close();

	
	return styledWriter.write(root);
}

bool LdapConfig::IsUpdateInfoVaild(const std::string &str_ldap_config_info)
{
	Json::Reader reader;
	Json::Value root;
	if( reader.parse(str_ldap_config_info, root))
	{
		if (!root.isMember("host_name"))
			return false;
		if (root["host_name"].asString() == "")
			return false;
		if (!root.isMember("address"))
			return false;
		if(root["address"].asString() == "")
			return false;
		else
			return true;
	}
	else  return false;
}

LdapConfig& LdapConfig::getInstance()
{
  static LdapConfig m;
  return m;
}
