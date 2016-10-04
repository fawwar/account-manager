#pragma once

#include <memory>
#include <string>

#include "AccountDefine.h"

using json = nlohmann::json;

namespace gorilla {
    namespace account{

        class User
        {
        public:
            
            User(const std::string& str_user_info);

            ~User();

            std::string Account() const;
            
            std::string Password() const;

            std::string EncryptedPassword() const;

            std::string Level() const;

            json Json() const;
        
            std::string JsonString() const;
            
            bool AddUser(int& out_n_sql_error);

            std::string UpdateUser(const std::string &str_user_info);

            bool DeleteUser();
                
        private:

            json m_json_response_user_info;
                
            std::string m_str_account;
            std::string m_str_password;
            std::string m_str_encrypted_password;
            std::string m_str_level;
            std::string m_str_description;

            bool IsInfoExsist(const json& j_user_info, const std::string& str_key,
                std::string& out_srt_info);

            void SetUserInfo(const std::string& str_user_info);

            void SetResponseUserInfo();
        };
    }
}



