#include "UserInfo.h"
#include "AccountDB.h"
#include "hash_codec.hpp"

#include "./import/json-develop/json_tools.hpp"
#include "gorilla/log/logger.h"
#include "gorilla/log/logger_config.h"


using namespace gorilla::log;

namespace
{
    const std::string UPDATE_KEY_FIELD = "account";
}

namespace gorilla {
    namespace account{

        User::User(std::shared_ptr<AccountDB> sp_account_db, const std::string& str_user_info)
            : m_sp_account_db(sp_account_db)
            , m_json_response_user_info(DEFAULT_RESPONSE_USER)
        {
            SetUserInfo(str_user_info);
        }

        User::~User()
        {
        }

        std::string User::Account() const
        {
            return m_str_account;
        }

        std::string User::Password() const
        {
            return m_str_password;
        }

        std::string User::EncryptedPassword() const
        {
            return m_str_encrypted_password;    
        }

        std::string User::Level() const
        {
            return m_str_level;
        }
        
        json User::Json() const
        {
            return m_json_response_user_info;
        }

        std::string User::JsonString() const
        {
            return Json().dump();
        }

        bool User::AddUser(int& out_n_sql_error)
        {
            return m_sp_account_db->Insert(AccountDB::USERS, m_json_response_user_info, out_n_sql_error);
        }

        std::string User::UpdateUser(const std::string &str_user_info)
        {
             

            SetUserInfo(str_user_info);
            m_sp_account_db->Update(AccountDB::USERS, UPDATE_KEY_FIELD,
                m_str_account, m_json_response_user_info);

            LOGGER_S(info) << "UpdateUser = " << m_str_account << " > " <<  str_user_info << ", " << m_str_level; 

            return JsonString();
        }

        bool User::DeleteUser()
        {
            return m_sp_account_db->Delete(AccountDB::USERS, UPDATE_KEY_FIELD, m_str_account);    
        }

        bool User::IsInfoExsist(const json& j_user_info, const std::string& str_key,
                std::string& out_srt_info)
        {
            auto it = j_user_info.find(str_key);
            if(it != j_user_info.end()){
                
                out_srt_info = *it;
                return true;
            }

            return false;
        }

        void User::SetUserInfo(const std::string& str_user_info)
        {
            LOGGER_S(info) << "SetUserInfo= " << str_user_info;
            json user_info = json::parse(str_user_info);

            IsKeyExsist(user_info, "account", m_str_account);
            IsKeyExsist(user_info, "levelName", m_str_level);
            IsKeyExsist(user_info, "encryptedPassword", m_str_encrypted_password);
            IsKeyExsist(user_info, "description", m_str_description);

            if(IsKeyExsist(user_info, "password", m_str_password)){
                m_str_encrypted_password = hash::TextEncodeHash(m_str_password); 
            }

            SetResponseUserInfo();  
        }

        void User::SetResponseUserInfo()
        {
            m_json_response_user_info["account"] = m_str_account;
            m_json_response_user_info["encryptedPassword"] = m_str_encrypted_password;
            m_json_response_user_info["levelName"] = m_str_level; 
            m_json_response_user_info["description"] = m_str_description;

            //LOGGER_S(info) << "m_json_response_user_info= " << m_json_response_user_info;
        }
        
    }
}
