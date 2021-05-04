#include <fstream>
#include <iostream>
#include <string>
#include <json/json.h>
#include "LdapConfig.h"
#include <boost/filesystem.hpp>

#include "gorilla/log/logger.h"
#include "gorilla/log/logger_config.h"

#ifdef WIN32
static const std::string configPATH = "./";// "C:/opt/ivar/var/";
#else
static const std::string configPATH = "./";
#endif
static const std::string configNAME = "ldap_config.json";

using namespace gorilla::log;

LdapConfig::LdapConfig()
:m_strConfigFilePath(configPATH + configNAME)
{

	if(boost::filesystem::exists(configPATH) == false)
		boost::filesystem::create_directories(configPATH);
          
	if(boost::filesystem::exists(m_strConfigFilePath) == false){
		std::fstream fp;
		Json::Value root;
		root["address"] = "";
		root["host_name"] = "";
		root["ldap_port"] = "";
		root["timeout"] = 5;
		fp.open(m_strConfigFilePath, std::ios::out);
		Json::StyledWriter styledWriter;
		fp << styledWriter.write(root);
		fp.close();
	}

	host_name = "192.168.0.248";
	port = 389;
	timeout = 5; //initial ldap_open timeout 

	#ifdef TELSTRA    // ldap initial permission 
	permission ="ewogICAgaXZhcjogewogICAgICAgIGNoYW5uZWxzOiB7CiAgICAgICAgICAgIGNoYW5uZWw6IFtdLAogICAgICAgICAgICAidGVtcGxhdGUiOiB7CiAgICAgICAgICAgICAgICAiYXNzaWduIjogIlNldHRpbmciLAogICAgICAgICAgICAgICAgImNhbWVyYSI6ICJSZWFkIiwKICAgICAgICAgICAgICAgICJjb25kaXRpb24iOiAiUmVhZCIsCiAgICAgICAgICAgICAgICAiaW5mb1NldHRpbmciOiAiU2V0dGluZyIsCiAgICAgICAgICAgICAgICAiaXZhRXZlbnQiOiAiUmVhZCIsCiAgICAgICAgICAgICAgICAicmVjb3JkU2V0dGluZyI6ICJTZXR0aW5nIiwKICAgICAgICAgICAgICAgICJzeXN0ZW1FdmVudCI6ICJSZWFkIiwKICAgICAgICAgICAgICAgICJ2aWRlbyI6ICJMaXZlK1BsYXliYWNrK0FyY2hpdmUiLAogICAgICAgICAgICAgICAgInZpc2libGUiOiB0cnVlCiAgICAgICAgICAgIH0KICAgICAgICB9LAogICAgICAgIHR2V2FsbHM6IHsKICAgICAgICAgICAgZW5hYmxlOiBmYWxzZSwKICAgICAgICB9LAogICAgICAgIGl2YTogewogICAgICAgICAgICBhbmFseXNpczogIkRlbnkiLAogICAgICAgIH0sCiAgICAgICAgZXZlbnRzOiB7CiAgICAgICAgICAgICJlbmFibGUiOiB0cnVlLAogICAgICAgICAgICAiZXZlbnRNYW5hZ2VyU2V0dGluZyI6ICJSZWFkIiwKICAgICAgICAgICAgInN1YnNjcmlwdGlvbiI6ICJSZWFkIgogICAgICAgIH0sCiAgICAgICAgc3lzdGVtOiB7CiAgICAgICAgICAgICJhY2NvdW50IjogIkRlbnkiLAogICAgICAgICAgICAiYmV0YSI6ICJEZW55IiwKICAgICAgICAgICAgImluZm8iOiAiUmVhZCIsCiAgICAgICAgICAgICJsbXMiOiAiUmVhZCIsCiAgICAgICAgICAgICJtbXMiOiAiRGVueSIsCiAgICAgICAgICAgICJyZWJvb3QiOiAiRGVueSIsCiAgICAgICAgICAgICJzdG9yYWdlIjogIlJlYWQiLAogICAgICAgICAgICAidGltZSI6ICJSZWFkIiwKICAgICAgICAgICAgInVwZ3JhZGUiOiAiRGVueSIsCiAgICAgICAgICAgICJzbWFydFJldGFpbFNlcnZpY2UiOiAiRGVueSIKICAgICAgICB9CiAgICB9LAogICAgdm1zOiB7CiAgICAgICAgdmlldzogIkRlbnkiCiAgICB9Cn0=";
	#else
	permission ="ewogICAgaXZhcjogewogICAgICAgIGNoYW5uZWxzOiB7CiAgICAgICAgICAgIGNoYW5uZWw6IFtdLAogICAgICAgICAgICAidGVtcGxhdGUiOiB7CiAgICAgICAgICAgICAgICAiYXNzaWduIjogIlNldHRpbmciLAogICAgICAgICAgICAgICAgImNhbWVyYSI6ICJSZWFkIiwKICAgICAgICAgICAgICAgICJjb25kaXRpb24iOiAiUmVhZCIsCiAgICAgICAgICAgICAgICAiaW5mb1NldHRpbmciOiAiU2V0dGluZyIsCiAgICAgICAgICAgICAgICAiaXZhRXZlbnQiOiAiUmVhZCIsCiAgICAgICAgICAgICAgICAicmVjb3JkU2V0dGluZyI6ICJTZXR0aW5nIiwKICAgICAgICAgICAgICAgICJzeXN0ZW1FdmVudCI6ICJSZWFkIiwKICAgICAgICAgICAgICAgICJ2aWRlbyI6ICJMaXZlK1BsYXliYWNrK0FyY2hpdmUiLAogICAgICAgICAgICAgICAgInZpc2libGUiOiB0cnVlCiAgICAgICAgICAgIH0KICAgICAgICB9LAogICAgICAgIHR2V2FsbHM6IHsKICAgICAgICAgICAgZW5hYmxlOiBmYWxzZSwKICAgICAgICB9LAogICAgICAgIGl2YTogewogICAgICAgICAgICBhbmFseXNpczogIkRlbnkiLAogICAgICAgIH0sCiAgICAgICAgZXZlbnRzOiB7CiAgICAgICAgICAgICJlbmFibGUiOiB0cnVlLAogICAgICAgICAgICAiZXZlbnRNYW5hZ2VyU2V0dGluZyI6ICJSZWFkIiwKICAgICAgICAgICAgInN1YnNjcmlwdGlvbiI6ICJSZWFkIgogICAgICAgIH0sCiAgICAgICAgc3lzdGVtOiB7CiAgICAgICAgICAgICJhY2NvdW50IjogIkRlbnkiLAogICAgICAgICAgICAiYmV0YSI6ICJEZW55IiwKICAgICAgICAgICAgImluZm8iOiAiUmVhZCIsCiAgICAgICAgICAgICJsbXMiOiAiUmVhZCIsCiAgICAgICAgICAgICJtbXMiOiAiRGVueSIsCiAgICAgICAgICAgICJyZWJvb3QiOiAiRGVueSIsCiAgICAgICAgICAgICJzdG9yYWdlIjogIlJlYWQiLAogICAgICAgICAgICAidGltZSI6ICJSZWFkIiwKICAgICAgICAgICAgInVwZ3JhZGUiOiAiRGVueSIsCiAgICAgICAgICAgICJzbWFydFJldGFpbFNlcnZpY2UiOiAiRGVueSIKICAgICAgICB9CiAgICB9LAogICAgdm1zOiB7CiAgICAgICAgdmlldzogIkRlbnkiCiAgICB9Cn0=";	
	#endif
	
	try 
	{
		ParseConfig();
	}
	catch(std::exception& e)
	{
		//file not exist or parse error
		std::cout << e.what() << std::endl;
	}
  
}

void LdapConfig::ParseConfig()  // parse to json
{
	
	Json::Value root;
	Json::Reader reader;
	if (reader.parse(Read(), root))
	{

		if (root.isMember("host_name"))
		{
			host_name = root["host_name"].asString();
			//LOGGER_S(info) << "LdapConfig.cpp host_name!!!!!!!!!" << host_name;
		}
		if (root.isMember("ldap_port"))
		{
			port = std::atoi(root["ldap_port"].asString().c_str());
			//LOGGER_S(info) << "LdapConfig.cpp ldap_port!!!!!!!!!" << port;
		}
		if (root.isMember("address"))
		{
			address = root["address"].asString();
			//LOGGER_S(info) << "LdapConfig.cpp address!!!!!!!!!" << address;
		}
		if (root.isMember("permission"))
		{
			permission = root["permission"].asString();
		}
		if (root.isMember("timeout") )
		{
			timeout = root["timeout"].asInt();	
		}

	}	

	
	DEFAULT_PERMISSIONS = json::object_t{
		{ "_", permission }
	};
	DEFAULT_ACCESSRIGHT = json::object_t{
		{ "accessRightName", "AD_user" },
		{ "permissions", DEFAULT_PERMISSIONS },
		{ "description", "" }
	};

}


std::string LdapConfig::Read()     //read config 
{
	std::ifstream ifs1("ldap_config.json");
	std::string text((std::istreambuf_iterator<char>(ifs1)),
		(std::istreambuf_iterator<char>()));
	
	return text;
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
				ldapConfig.host_name = root["host_name"].asString();
				root["host_name"] = ldapConfig.host_name;
				//LOGGER_S(info)<<"Error AccountManager::UpdateLdapConfig host_name!!!!!!!!!" << ldapconfig.host_name;
			}
			if (root.isMember("ldap_port"))
			{
				ldapConfig.port = std::stoi(root["ldap_port"].asString());
				//ldapConfig.port = root["ldap_port"].asInt();
				root["ldap_port"] = std::to_string(ldapConfig.port);
				//LOGGER_S(info) << "Error AccountManager::UpdateLdapConfig ldap_port!!!!!!!!!" << ldapconfig.ldap_port;
			}
                        if (root.isMember("address"))
			{
				ldapConfig.address = root["address"].asString();
				root["address"] = ldapConfig.address;
				//LOGGER_S(info) << "Error AccountManager::UpdateLdapConfig ldap_port!!!!!!!!!" << ldapconfig.ldap_port;
			}
			if (root.isMember("timeout"))
			{
				ldapConfig.timeout = root["timeout"].asInt();
				root["timeout"] = ldapConfig.timeout;
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
		if(!root.isMember("ldap_port"))
			return false;
		if(root["ldap_port"].asString() == "")
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
