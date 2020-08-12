#include <iostream>
#include <string.h>
#include <stdio.h>
#include "gorilla/log/logger.h"
#include "gorilla/log/logger_config.h"
#include "LdapConfig.h"

#ifdef WIN32
#include<Windows.h>
#include<winldap.h>
#include<stdio.h>

#else     //openldap    linux

extern "C" {
# define LDAP_DEPRECATED 1
# include <ldap.h>
# include <lber.h>
}

#endif 


using namespace gorilla::log;


class IAuthenticator {	
	public:
		IAuthenticator();
		
		virtual ~IAuthenticator(){}

		virtual bool AuthenticateActiveDirectory( const std::string& str_ldap_account ,const std::string& str_password) = 0;
			
};

class LdapConnection{
	public:
	    LdapConnection();
	    ~LdapConnection();
	    LDAP* pLdapConnection = NULL;
};

class LdapAuthenticator : public  IAuthenticator  {
public:	
	LdapAuthenticator();
	~LdapAuthenticator();
	
	void IsLdapOpen();
	bool AuthenticateActiveDirectory (const std::string& str_ldap_account, const std::string& str_password );
	LdapConnection conn;

private:
	//LDAP* pLdapConnection = NULL;
	#ifdef WIN32
	ULONG lRtn = 0;
	ULONG version = LDAP_VERSION3;
    
	#else
	int lRtn = 0;
	int version = LDAP_VERSION3;		
	
	#endif 
	
	LdapConfig &ldapConfig = LdapConfig::getInstance();	
	
};







 





