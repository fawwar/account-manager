#include "LdapAuthentication.h"
#include "gorilla/log/logger.h"
#include "gorilla/log/logger_config.h" 
#include <json/json.h>
#include "LdapConfig.h"
#include <string>
#include <fstream>  


using namespace gorilla::log;


IAuthenticator::IAuthenticator()
{
	LOGGER_S(info) << "IAuthenticator::IAuthenticator()";
}

/*
IAuthenticator::~IAuthenticator()
{
	LOGGER_S(info) << "IAuthenticator::~IAuthenticator()";
}
*/

LdapConnection::LdapConnection()
{
        LdapConfig &ldapConfig = LdapConfig::getInstance();
        pLdapConnection = ldap_init((char*)ldapConfig.host_name.c_str(), ldapConfig.port);
        //LOGGER_S(info) << "LdapAuthenticator::LdapAuthenticator " << ldapConfig.host_name.c_str();
        //LOGGER_S(info) << "LdapAuthenticator::LdapAuthenticator " << ldapConfig.ldap_port;


        if (pLdapConnection == NULL)
        {
                throw std::runtime_error("ldap_init failed") ;
        }
        else
        {
                LOGGER_S(info) << "ldap_init succeeded";
        }



}

LdapConnection::~LdapConnection()
{
        ldap_unbind(pLdapConnection);
}


LdapAuthenticator::LdapAuthenticator()
{
	LOGGER_S(info) << "LdapAuthenticator::LdapAuthenticator()";

	lRtn = ldap_set_option(
		conn.pLdapConnection,           // Session handle
		LDAP_OPT_PROTOCOL_VERSION, // Option
		(void*)&version);         // Option value
	if (lRtn == LDAP_SUCCESS)
	{
		LOGGER_S(info)<<"ldap version set to 3.0";
	}
	else
	{
		LOGGER_S(info)<<"SetOption Error:" << lRtn;
		throw std::runtime_error("ldap_setOption error ");
	}
	
}


LdapAuthenticator::~LdapAuthenticator()
{
	LOGGER_S(info) << "LdapAuthenticator::~LdapAuthenticator()";
	
}

void LdapAuthenticator::IsLdapOpen()
{	 
                //LdapConfig &ldapConfig = LdapConfig::getInstance();
                LDAP* pLdapConnection = NULL;
                pLdapConnection = ldap_open((char*)ldapConfig.host_name.c_str(),ldapConfig.port);
                if(pLdapConnection == NULL)
                {
                        LOGGER_S(info) << "ldap_open failed ";
                        //ldap_unbind(pLdapConnection);
			throw std::runtime_error ("ldap_open error");
                }
                else
                {
                        LOGGER_S(info) << "ldap_open succeeded";
                }	
}


bool LdapAuthenticator::AuthenticateActiveDirectory( const std::string& str_ldap_account,const std::string& str_password )
{
	std::string str_username = str_ldap_account + ldapConfig.address;
	
	lRtn = ldap_simple_bind_s(conn.pLdapConnection, (char*)str_username.c_str(), (char*)str_password.c_str());
	
	if(lRtn == LDAP_SUCCESS)
	{
	    LOGGER_S(info) << "ldap_simple_bind_s success";
	    return true;
	}
	else 
	{
	    LOGGER_S(info) << "ldap_simple_bind_s failed with 0x" << lRtn; 
	    return false;
	}

}






