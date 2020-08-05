#include "LdapAuthentication.h"

#include "gorilla/log/logger.h"
#include "gorilla/log/logger_config.h"
//#include <json/json.h> 

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

#ifdef WIN32

LdapConnection::LdapConnection()
{
	LdapConfig &ldapConfig = LdapConfig::getInstance();
	pLdapConnection = ldap_init((char*)ldapConfig.host_name.c_str(), ldapConfig.port);
	//LOGGER_S(info) << "LdapAuthenticator::LdapAuthenticator " << ldapConfig.host_name.c_str();
	//LOGGER_S(info) << "LdapAuthenticator::LdapAuthenticator " << ldapConfig.ldap_port;


		if (pLdapConnection == NULL)
		{
			//LOGGER_S(info) << "ldap_init failed with 0x" << LdapGetLastError();
			throw std::runtime_error("ldap_init failed with 0x ") ;
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

	lRtn = ldap_connect(conn.pLdapConnection, NULL);
	if (lRtn == LDAP_SUCCESS)
	{
		LOGGER_S(info)<<"ldap_connect succeeded";
	}
	else
	{
		LOGGER_S(info)<<"ldap_connect faied with 0x"<<lRtn;
		throw std::runtime_error("ldap_connect fail ");
		
	}

}


LdapAuthenticator::~LdapAuthenticator()
{
	LOGGER_S(info) << "LdapAuthenticator::~LdapAuthenticator()";
	
}

bool LdapAuthenticator::AuthenticateActiveDirectory( const std::string& str_ldap_account, const std::string& str_password) 
{
		LdapConfig &ldapConfig = LdapConfig::getInstance();
		std::string str_username = str_ldap_account + ldapConfig.address;
		lRtn = ldap_simple_bind_s(conn.pLdapConnection, (char*)str_username.c_str(), (char*)str_password.c_str());

		if (lRtn == LDAP_SUCCESS)
		{
			LOGGER_S(info) <<"ldap_simple_bind_s succeeses";
			return true;
		}
		else
		{	
			LOGGER_S(info) << "ldap_simple_bind_s failed with 0x" << lRtn;
			//ldap_unbind(pLdapConnection);
			return false;
		}
}

#else    // linux 

#include "LDAPConnection.h"
#include "LDAPConstraints.h"
#include "LDAPSearchReference.h"
#include "LDAPSearchResults.h"
#include "LDAPAttribute.h"
#include "LDAPAttributeList.h"
#include "LDAPEntry.h"
#include "LDAPException.h"
#include "LDAPModification.h"
#include "debug.h"

#include "ldap.h"
#include "lber.h"
#define LDAP_DEPRECATED 1

LdapAuthenticator::LdapAuthenticator()
{
	LOGGER_S(info)<< "LdapAuthenticator::LdapAuthenticator";
	
	LdapConfig &ldapConfig = LdapConfig::getInstance();
	
        try{
	    cons=new LDAPConstraints; 
	    lc=new LDAPConnection(ldapConfig.host_name,ldapConfig.port,cons);
           }catch(LDAPException &e)
	    {
	     LOGGER_S(info)<<"-----------constructor caught Exeception --------";
	     LOGGER_S(info)<<e;
	     throw ;	
	    }


}

LdapAuthenticator::~LdapAuthenticator()
{
	LOGGER_S(info)<<"LdapAuthenticator::~LdapAuthenticator";
	
	lc->unbind();
	delete lc;

}


bool LdapAuthenticator::AuthenticateActiveDirectory( const std::string& str_ldap_account,const std::string& str_password )
{	/*
		LdapConfig &ldapConfig1 = LdapConfig::getInstance();
		LDAP* pLdapConnection = NULL;
		pLdapConnection = ldap_open(ldapConfig1.host_name,ldapConfig1.port);
		if(pLdapConnection == NULL)
		{	
			LOGGER_S(info) << "ldap_init failed with 0x";
			ldap_unbind(pLdapConnection);
		}
		else 
		{
			LOGGER_S(info) << "ldap_init succeeded";
		}
	  */                 	
                    LOGGER_S(info) << "----------------doing bind --------";
                    //std::string str_username  = str_ldap_account +"@gorillascience.com";
		    LdapConfig &ldapConfig = LdapConfig::getInstance();
                    std::string str_username  = str_ldap_account +ldapConfig.address;
                    try{
			/*
			ctrls->add(LDAPCtrl(LDAP_CONTROL_MANAGEDSAIT));
           		cons->setServerControls(ctrls); 
            		lc->setConstraints(cons);
			*/
                        lc->bind(str_username , str_password);
                        LOGGER_S(info) <<"lc->getHost()" << lc->getHost();
			return true;
                       }catch (LDAPException &e){
                        LOGGER_S(info) <<"----------------caught Exception ------------";
                        LOGGER_S(info) << e ;
			return false;
                    }


}
bool LdapAuthenticator::IsLDAPConnected ( std::string host_name, int port)
{
	 
                LdapConfig &ldapConfig1 = LdapConfig::getInstance();
                LDAP* pLdapConnection = NULL;
                pLdapConnection = ldap_open(host_name.c_str(),port);
                if(pLdapConnection == NULL)
                {
                        LOGGER_S(info) << "ldap_init failed with 0x";
                        //ldap_unbind(pLdapConnection);
			return false;
                }
                else
                {
                        LOGGER_S(info) << "ldap_init succeeded";
			return true;
                }
          

}

#endif


