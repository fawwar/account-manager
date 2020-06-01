#pragma once

// std
#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "Error.h"
#include "AccountDefine.h"


#include "LegacyHttp.h"

 

namespace gorilla {
    namespace account{

        class User;
        class AccessRight;
        class AccountManager
        {
        public:

            AccountManager();

            ~AccountManager();

            bool GetUserAccessRight(const std::string str_account, std::string& out_str_access_right);

            bool VerifyAccount(const std::string& str_account, const std::string& str_password);
#ifdef LDAP_OPTION
			Error GetLdapConfig(std::string &out_str_reply);   //Get ldap info

			Error UpdateLdapConfig(const std::string &str_ldap_config_info, std::string &out_str_reply);   //Update ldap info
#endif
            Error GetUsers(std::string &out_str_reply);

            Error AddUser(const std::string &str_user_info, std::string &out_str_reply);

            Error GetUser(const std::string &str_account, std::string &out_str_reply);

            Error UpdateUser(const std::string &str_account, const std::string &str_login_level, 
                const std::string &str_user_info, std::string &out_str_reply, const Server::request& request);

            Error DeleteUser(const std::string &str_account, std::string &out_str_reply);

            Error GetUserPermissions(const std::string &str_account, const std::list<std::string>& lst_fields,
                std::string &out_str_reply);

            Error GetAccessRights(std::string &out_str_reply);

            Error AddAccessRight(const std::string &str_access_right_info, std::string &out_str_reply);

            Error GetAccessRight(const std::string &str_access_right_name, std::string &out_str_reply);

            Error UpdateAccessRight(const std::string &str_access_right_name, const std::string &str_access_right_info,
                std::string &out_str_reply);

            Error DeleteAccessRight(const std::string &str_access_right_name, std::string &out_str_reply);
            
        private:

            ErrorReply m_error_reply;
            
            mutable std::mutex m_mux_users;
            std::map<std::string, std::shared_ptr<User>> m_map_users;
            mutable std::mutex m_mux_access_rights;
            std::map<std::string, std::shared_ptr<AccessRight>> m_map_access_rights;

		mutable std::mutex m_mux_ldapconfig;
		

            

            void CreateUsersInfo();

            void CreateLevelsInfo();

            bool IsAccountVaild(const std::string &str_user_info, std::string &out_str_reply);

            bool IsPasswordVaild(const std::string &str_user_info, std::string &out_str_reply);

            bool IsAccessRightNameVaild(const std::string &str_user_info, std::string &out_str_reply);

            bool IsUserInfoVaild(const std::string &str_user_info, std::string &out_str_reply);

            bool IsAccessRightNameExist(const std::string &str_user_info);

            bool IsAccessRightNameBeUsing(const std::string &str_access_right_name);

        };
    }
}


