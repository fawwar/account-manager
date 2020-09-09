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
		throw std::runtime_error("ldap_setOption failed ");
	}
	
}


LdapAuthenticator::~LdapAuthenticator()
{
	LOGGER_S(info) << "LdapAuthenticator::~LdapAuthenticator()";
	
}

bool LdapAuthenticator::IsLdapOpen()
{	
 	//ldap* pldapconnection = NULL;
	//int opt_timeout = 2;
	//int timelimit  = 2;
	//int network_timeout = 2; 
	/*
	if(opt_timeout > 0 )
	{
	    struct timeval optTimeout;
	    optTimeout.tv_usec = 0;
	    optTimeout.tv_sec = opt_timeout;

	    lRtn = ldap_set_option (pLdapConnection, LDAP_OPT_TIMEOUT, (void * )& optTimeout);
	    if(lRtn != LDAP_OPT_SUCCESS)
	    {
		LOGGER_S(info) << "LDAP_OPT_TIMEOUT false";
	    }
	    else
	    {
		LOGGER_S(info) << "LDAP_OPT_TIMEOUT true";
	    }		
	}	 
	
	if (timelimit > 0)
	{
	   	    
	    lRtn = ldap_set_option (pLdapConnection, LDAP_OPT_TIMELIMIT, (void * )&timelimit);
	    if (lRtn != LDAP_OPT_SUCCESS)
	    {
		LOGGER_S(info) << "LDAP_OPT_TIMELIMIT false" ; 
	    }
	    else 
	    {
		LOGGER_S(info) << "LDAP_OPT_TIMELIMIT  true";
	    }

	}
	*/
	ldap* pLdapConnection = NULL;
#ifdef WIN32
	if(ldapConfig.timeout >= 2)
	{
	    //int timeout = 2;
	    l_timeval ldap_connect_timeout;
	    ldap_connect_timeout.tv_sec = ldapConfig.timeout;
	    ldap_connect_timeout.tv_usec = 0;

	    lRtn = ldap_connect(conn.pLdapConnection, &ldap_connect_timeout);   //NULL
	    if (lRtn == LDAP_SUCCESS)
	    {
		LOGGER_S(info) << "lap_connect succeeded";
		return true;
	    }
	    else
	    {	
		LOGGER_S(info) << "ldap_connect faied with 0x" << lRtn;
		throw std::runtime_error("ldap_open failed");
		return false;
	    }  
	}
#else
	//int network_timeout =2;
	if (ldapConfig.timeout >= 2)
	{
	    struct timeval networkTimeout;
	    networkTimeout.tv_usec = 0;
	    networkTimeout.tv_sec = ldapConfig.timeout;

	    lRtn = ldap_set_option(pLdapConnection, LDAP_OPT_NETWORK_TIMEOUT, (void*)&networkTimeout);   // LDAP_OPT_NETWORK_TIMEOUT
	    if(lRtn != LDAP_SUCCESS)
	    {
		//LOGGER_S(info) << "LDAP_OPT_NETWORK_TIMEOUT false";
	    }
	    else 
	    {
		//LOGGER_S(info) << "LDAP_OPT_NETWORK_TIMEOUT true";
	    }

		pLdapConnection = ldap_open((char*)ldapConfig.host_name.c_str(),ldapConfig.port);	
			
                if(pLdapConnection == NULL)
                {
                        LOGGER_S(info) << "ldap_open failed ";
			throw std::runtime_error ("ldap_open failed");
			return false;
                }
                else
                {
                        LOGGER_S(info) << "ldap_open succeeded";
			return true;
                }	

	 }
		
#endif	
                
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






