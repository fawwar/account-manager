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

LdapAuthenticator::LdapAuthenticator()
{
	LOGGER_S(info) << "LdapAuthenticator::LdapAuthenticator()";
	LdapConfig ldapConfig;
	pLdapConnection = ldap_init(const_cast<char *>(ldapConfig.host_name.c_str()), ldapConfig.ldap_port);
	if (pLdapConnection == NULL)
	{       
		LOGGER_S(info)<<"ldap_init failed with 0x"<<LdapGetLastError();  
		ldap_unbind(pLdapConnection);
	}
	else
	{
		LOGGER_S(info)<<"ldap_init succeeded";
	}

	lRtn = ldap_set_option(
		pLdapConnection,           // Session handle
		LDAP_OPT_PROTOCOL_VERSION, // Option
		(void*)&version);         // Option value
	if (lRtn == LDAP_SUCCESS)
	{
		LOGGER_S(info)<<"ldap version set to 3.0";
	}
	else
	{
		LOGGER_S(info)<<"SetOption Error:" << lRtn;
		ldap_unbind(pLdapConnection);
	}

	lRtn = ldap_connect(pLdapConnection, NULL);
	if (lRtn == LDAP_SUCCESS)
	{
		LOGGER_S(info)<<"ldap_connect succeeded";
	}
	else
	{
		LOGGER_S(info)<<"ldap_connect faied with 0x"<<lRtn;
		ldap_unbind(pLdapConnection);
	}

}


LdapAuthenticator::~LdapAuthenticator()
{
	LOGGER_S(info) << "LdapAuthenticator::~LdapAuthenticator()";
	ldap_unbind(pLdapConnection);
}

bool LdapAuthenticator::AuthenticateActiveDirectory(const std::string& str_account, const std::string& str_password, const std::string& str_ldap_account)
{
	//if (str_account.substr(0, str_ldap.size()) == str_ldap)
	//{
		//std::string str_ldap_account = str_account.substr(str_ldap.size());
		std::string str_username = str_ldap_account + "@gorillascience.com";
		//LOGGER() << "str_username " << str_username;
		//char *username = str_username.c_str();
		char *username = new char[str_username.length() + 1];
		strcpy(username, str_username.c_str());
		char *password = new char[str_password.length() + 1];
		strcpy(password, str_password.c_str());

		lRtn = ldap_simple_bind_s(pLdapConnection, username, password);

		delete[] username;
		delete[] password;
		if (lRtn == LDAP_SUCCESS)
		{
			LOGGER_S(info) <<"ldap_simple_bind_s succeeses";
			return true;
		}
		else
		{	
			LOGGER_S(info) << "ldap_simple_bind_s failed with 0x" << lRtn;
			ldap_unbind(pLdapConnection);
			return false;
		}
	//}
	//else {
	//	return false;
	//}
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


LdapAuthenticator::LdapAuthenticator()
{
	LOGGER_S(info)<< "LdapAuthenticator::LdapAuthenticator";
	LdapConfig ldapConfig;
	cons=new LDAPConstraints;
	ctrls=new LDAPControlSet;
	lc=new LDAPConnection(ldapConfig.host_name,ldapConfig.ldap_port);
	
        try{
            ctrls->add(LDAPCtrl(LDAP_CONTROL_MANAGEDSAIT));
            cons->setServerControls(ctrls); 
            lc->setConstraints(cons);
	}catch(LDAPException &e)
	{
	 LOGGER_S(info)<<"-----------constructor caught Exeception --------";
	 LOGGER_S(info)<<e;	
	}


}

LdapAuthenticator::~LdapAuthenticator()
{
	LOGGER_S(info)<<"LdapAuthenticator::~LdapAuthenticator";
	
	lc->unbind();
	delete lc;

}


bool LdapAuthenticator::AuthenticateActiveDirectory(const std::string& str_account, const std::string& str_password, const std::string& str_ldap_account)
{	
	//if (str_account.substr(0,str_ldap.size()) == str_ldap)
        //        {
                    //std::string str_ldap_account = str_account.substr(str_ldap.size());
                    //LOGGER_S(info)<<"str_ldap_accont " <<str_ldap_account;
                    	
                    LOGGER_S(info) << "----------------doing bind --------";
                    std::string str_username  = str_ldap_account +"@gorillascience.com";
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
		//return true;
                //}

	//else 
	//{	
	//	return false;
	//}	

}

#endif


