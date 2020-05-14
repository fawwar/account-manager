#include <iostream>
#include <string.h>
#include <stdio.h>
#include "gorilla/log/logger.h"
#include "gorilla/log/logger_config.h"
#include "LdapConfig.h"

using namespace gorilla::log;

class IAuthenticator {	
	public:
		IAuthenticator();
		
		virtual ~IAuthenticator(){}
	protected:
		//virtual bool IAuthenticator_connection(const std::string& str_account, const std::string& str_password)=0;
		virtual bool AuthenticateActiveDirectory(const std::string& str_account, const std::string& str_password, const std::string& str_ldap_account) = 0;
		
};



#ifdef WIN32
#include<Windows.h>
#include<winldap.h>
#include<stdio.h>

class LdapAuthenticator : IAuthenticator  {
public:	
	LdapAuthenticator();
	~LdapAuthenticator();

	bool AuthenticateActiveDirectory (const std::string& str_account, const std::string& str_password);

private:
	LDAP* pLdapConnection = NULL;
	ULONG lRtn = 0;
	int version = LDAP_VERSION3;
	
};

#else
//openldap    linux


//class LdapConfig;
class LDAPConstraints;
class LDAPControlSet;
class LDAPConnection;
class LdapAuthenticator:IAuthenticator{

public:
        LdapAuthenticator();
        ~LdapAuthenticator();

        bool AuthenticateActiveDirectory(const std::string& str_account, const std::string& str_password, const std::string& str_ldap_account);
private:
	LDAPConstraints* cons;
        LDAPControlSet* ctrls;
	LDAPConnection *lc;
		
	

};

 

#endif 


