#pragma once

// std
#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "Error.h"
#include "AccountDefine.h"

namespace gorilla {
    namespace account{

        class User;
        class Level;
        class AccountManager
        {
        public:

            AccountManager();

            ~AccountManager();

            bool GetUserLevel(const std::string str_account, std::string& out_str_level);

            bool VerifyAccount(const std::string& str_account, const std::string& str_password);

            Error GetUsers(std::string &out_str_reply);

            Error AddUser(const std::string &str_user_info, std::string &out_str_reply);

            Error GetUser(const std::string &str_account, std::string &out_str_reply);

            Error UpdateUser(const std::string &str_account, const std::string &str_login_level, 
                const std::string &str_user_info, std::string &out_str_reply);

            Error DeleteUser(const std::string &str_account, std::string &out_str_reply);

            Error GetUserFeatures(const std::string &str_account, const std::list<std::string>& lst_fields,
                std::string &out_str_reply);

            Error GetLevels(std::string &out_str_reply);

            Error AddLevel(const std::string &str_level_info, std::string &out_str_reply);

            Error GetLevel(const std::string &str_level_name, std::string &out_str_reply);

            Error UpdateLevel(const std::string &str_level_name, const std::string &str_level_info,
                std::string &out_str_reply);

            Error DeleteLevel(const std::string &str_level_name, std::string &out_str_reply);
            
        private:

            ErrorReply m_error_reply;
            
            mutable std::mutex m_mux_users;
            std::map<std::string, std::shared_ptr<User>> m_map_users;
            mutable std::mutex m_mux_levels;
            std::map<std::string, std::shared_ptr<Level>> m_map_levels;

            

            void CreateUsersInfo();

            void CreateLevelsInfo();

            bool IsAccountVaild(const std::string &str_user_info, std::string &out_str_reply);

            bool IsPasswordVaild(const std::string &str_user_info, std::string &out_str_reply);

            bool IsLevelNameVaild(const std::string &str_user_info, std::string &out_str_reply);

            bool IsUserInfoVaild(const std::string &str_user_info, std::string &out_str_reply);

            bool IsLevelNameExist(const std::string &str_user_info);

            bool IsLevelNameBeUsing(const std::string &str_level_name);

        };
    }
}


