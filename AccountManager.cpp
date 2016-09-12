//#include <regex>
#include <list>
#include <sstream>
#include <boost/regex.hpp>

#include "AccountManager.h"
#include "AccountDB.h"
#include "UserInfo.h"
#include "LevelInfo.h"

#include "hash.h"
#include "./import/json-develop/json_tools.hpp"
#include "gorilla/log/logger.h"
#include "gorilla/log/logger_config.h"


using namespace gorilla::log;

namespace gorilla {
    namespace account{

        AccountManager::AccountManager()
            : m_sp_account_db(AccountDB::Instance())
        {
            CreateUsersInfo();
            CreateLevelsInfo();
        }

        AccountManager::~AccountManager()
        {
        }

        bool AccountManager::GetUserLevel(const std::string str_account, std::string& out_str_level)
        {
            auto it = m_map_users.find(str_account);
            if (it != m_map_users.end()){

                out_str_level = it->second->Level();
                return true;
            }

            return false;
        }

        bool AccountManager::VerifyAccount(const std::string& str_account, const std::string& str_password)
        {
             LOGGER_S(info) << "Verify = " << str_account << "," << str_password;

             std::lock_guard<std::mutex> autoLock(m_mux_users);   
             auto it = m_map_users.find(str_account);
             if (it != m_map_users.end()){

                /* 20 bytes hash text = 16 bytes(md5) + 4 bytes(salt)*/
                std::string password = it->second->EncryptedPassword();

                LOGGER_S(info) << "SQL Pass = " << password;

                std::string str_salt = password.substr(MD5LEN*2);
                std::string str_md5_password = password.substr(0, MD5LEN*2);

                /* parser last 4 bytes salt */
                unsigned long ul_salt;
                std::istringstream iss(str_salt);
                iss >> std::hex >> ul_salt;
                LOGGER_S(info) << "SQL Salt = " << str_salt;

                /* use original password and 4 bytes salt to decode md5 text */
                const size_t PASSWORD_LEN = str_password.size();
                unsigned char* uc_password = new unsigned char[PASSWORD_LEN];
                char decode_md5_password[MD5LEN * 2 + 1] = {00};
                memcpy(uc_password, str_password.c_str(), PASSWORD_LEN);

                TextMD5(ul_salt, uc_password, PASSWORD_LEN, decode_md5_password);
                delete []uc_password;  

                printf("Decode MD5 Pass = %s ", decode_md5_password);
                
                if(str_md5_password == decode_md5_password){

                    LOGGER_S(info) << ">>>> Compare Success  ";
                    return true;
                }
             }

             return false;
        }

        Error AccountManager::GetUsers(std::string &out_str_reply)
        {
            Error errorCode(INTERNAL_SERVER_ERROR);

            bool user_exist(false);
            json::array_t j_user_array;
            json j_users;
            
            std::lock_guard<std::mutex> autoLock(m_mux_users);
            for(auto& it : m_map_users){

                json j_user = it.second->Json();
                j_user_array.push_back(j_user);
                user_exist = true;
            }

            /* send device list */
            if(user_exist){

                errorCode = SUCCESS_RESPONSE;
                j_users = j_user_array;
                out_str_reply = j_users.dump();
            }
            else{

                errorCode = NAME_NOT_FOUND;      
                out_str_reply = m_error_reply.GetError("Haven't Any User", "<AccountManager::GetUsers> NAME_NOT_FOUND");
            }

            return errorCode;
        }

        Error AccountManager::AddUser(const std::string &str_user_info, std::string &out_str_reply)
        {
            Error errorCode(INTERNAL_SERVER_ERROR); 

            /* check level name exist in level map */
            if(!IsLevelNameExist(str_user_info)){

                out_str_reply = m_error_reply.GetError("LevelName Is Not Exist","<AccountManager::AddUser> FORBIDDEN");
                return FORBIDDEN;
            }
                
            
            /* check account,password,level content is vaild */
            if(!IsUserInfoVaild(str_user_info, out_str_reply)){

                out_str_reply = m_error_reply.GetError(out_str_reply,"<AccountManager::AddUser> FORBIDDEN");
                return FORBIDDEN;
            }

            /* create user map */
            auto user = std::make_shared<User>(m_sp_account_db, str_user_info);
            {            
                std::lock_guard<std::mutex> autoLock(m_mux_users);
                std::string account = user->Account();

                int sql_error;
                if(!user->AddUser(sql_error)){

                    if((SQLError)sql_error == CONSTRAINT){
                        out_str_reply = m_error_reply.GetError("DataBase Have This Account","<AccountManager::AddUser> FORBIDDEN");
                        errorCode = FORBIDDEN;
                    }
                    else
                        out_str_reply = m_error_reply.GetError("Insert To Database Error","<AccountManager::AddUser> INTERNAL_SERVER_ERROR");
                    
                    return errorCode;      
                }

                m_map_users.insert(std::pair<std::string, std::shared_ptr<User> >(account, user));
                out_str_reply = user->JsonString();
                errorCode = SUCCESS_RESPONSE;            
            }
            
            return errorCode;
        }

        Error AccountManager::GetUser(const std::string &str_account, std::string &out_str_reply)
        {
            Error errorCode(INTERNAL_SERVER_ERROR); 
            std::lock_guard<std::mutex> autoLock(m_mux_users);
            auto it = m_map_users.find(str_account.c_str());
            if (it != m_map_users.end()){

                errorCode = SUCCESS_RESPONSE;
                out_str_reply = it->second->JsonString();
            }
            else{

                errorCode = NAME_NOT_FOUND; 
                out_str_reply = m_error_reply.GetError("User Name Not Found","<AccountManager::GetUser> NAME_NOT_FOUND");
            }
                 
            return errorCode;
        }

        Error AccountManager::UpdateUser(const std::string &str_account, const std::string &str_login_level, 
                const std::string &str_user_info, std::string &out_str_reply)
        {
            Error errorCode(INTERNAL_SERVER_ERROR); 

            /* check level name exist in level map */
            if(!IsLevelNameExist(str_user_info)){

                out_str_reply = m_error_reply.GetError("LevelName Is Not Exist","<AccountManager::UpdateUser> FORBIDDEN");
                return FORBIDDEN;
            }

            json info = json::parse(str_user_info); 
            std::lock_guard<std::mutex> autoLock(m_mux_users);
            auto it = m_map_users.find(str_account.c_str());
            if (it != m_map_users.end()){

                 if(str_login_level == "admin"){

                     /* admin account only change password */   
                     if(str_account == "admin"){

                        if(IsKeyExsist(info, "account") || IsKeyExsist(info, "levelName")){

                            out_str_reply = m_error_reply.GetError("User No Permissions To Chang Account Or LevelName", 
                                "<AccountManager::UpdateUser> FORBIDDEN");
                            
                            return FORBIDDEN;
                        }
                     }
                    
                     /* check userinfo vaild */   
                     if(!IsUserInfoVaild(str_user_info, out_str_reply))
                        return FORBIDDEN;

                     /* update userinfo */
                     errorCode = SUCCESS_RESPONSE;   
                     out_str_reply = it->second->UpdateUser(str_user_info);

                     /* if account change reset map */   
                     if(it->second->Account() != str_account){   
                        m_map_users.insert(std::pair<std::string, std::shared_ptr<User> >(it->second->Account(), it->second));
                        m_map_users.erase(it);
                     }
                 }
                 else{
   
                    /* user account only change password */
                    if(IsKeyExsist(info, "account") || IsKeyExsist(info, "levelName")){

                        out_str_reply = m_error_reply.GetError("User No Permissions To Chang Account Or LevelName",
                            "<AccountManager::UpdateUser> FORBIDDEN");
                        
                        return FORBIDDEN;
                    }
                     
                    /* check password vaild */
                    if (!IsPasswordVaild(str_user_info, out_str_reply)){
                        out_str_reply = m_error_reply.GetError("User Password Invaild","<AccountManager::UpdateUser> FORBIDDEN"); 
                        return FORBIDDEN;
                    }
                    
                    errorCode = SUCCESS_RESPONSE;   
                    out_str_reply = it->second->UpdateUser(str_user_info);
                 }

            }
            else{

                LOGGER_S(debug) << "Get User Fail, User Name Not Found \n";
                errorCode = NAME_NOT_FOUND; 
                out_str_reply = m_error_reply.GetError("User Name Not Found","<AccountManager::UpdateUser> NAME_NOT_FOUND");
            }
                 
            return errorCode;
        }

        Error AccountManager::DeleteUser(const std::string &str_account, std::string &out_str_reply)
        {
            Error errorCode(INTERNAL_SERVER_ERROR); 
            std::lock_guard<std::mutex> autoLock(m_mux_users);
            auto it = m_map_users.find(str_account.c_str());
            if (it != m_map_users.end()){

                if(!it->second->DeleteUser()){
                    
                    out_str_reply = m_error_reply.GetError("Delete To Database Error", "<AccountManager::DeleteUser> INTERNAL_SERVER_ERROR");
                    return errorCode;      
                }
                errorCode = PET_DELETE;
                m_map_users.erase(it);
            }
            else{

                errorCode = NAME_NOT_FOUND; 
                out_str_reply = m_error_reply.GetError("User Name Not Found", "<AccountManager::DeleteUser> NAME_NOT_FOUND");
            }
                 
            return errorCode;
        }

        Error AccountManager::GetUserFeatures(const std::string &str_account, const std::list<std::string>& lst_fields,
                std::string &out_str_reply)
        {
            Error errorCode(INTERNAL_SERVER_ERROR);

            std::unique_lock<std::mutex> uniquelock(m_mux_users);
            auto it_user = m_map_users.find(str_account.c_str());
            if (it_user != m_map_users.end()){

                std::string level_name = it_user->second->Level();
                uniquelock.unlock();

                /* find level name featues */
                std::lock_guard<std::mutex> autoLock(m_mux_levels);
                auto it_level = m_map_levels.find(level_name.c_str());
                if (it_level != m_map_levels.end()){

                    out_str_reply = it_level->second->Features(lst_fields);
                    errorCode = SUCCESS_RESPONSE;           
                } 
            }
            else{

                errorCode = NAME_NOT_FOUND; 
                out_str_reply = m_error_reply.GetError("User Name Not Found","<AccountManager::GetUserFeatures> NAME_NOT_FOUND");
            }

            return errorCode;
        }

        Error AccountManager::GetLevels(std::string &out_str_reply)
        {
            Error errorCode(INTERNAL_SERVER_ERROR);

            bool level_exist(false);
            json::array_t j_level_array;
            json j_levels;
            
            std::lock_guard<std::mutex> autoLock(m_mux_levels);
            for(auto& it : m_map_levels){

                json j_level = it.second->Json();
                j_level_array.push_back(j_level);
                level_exist = true;
            }

            /* send device list */
            if(level_exist){

                errorCode = SUCCESS_RESPONSE;
                j_levels = j_level_array;
                out_str_reply = j_levels.dump();
            }
            else{
    
                errorCode = NAME_NOT_FOUND; 
                out_str_reply = m_error_reply.GetError("Haven't Any Level","<AccountManager::GetLevels> NAME_NOT_FOUND");
            }

            return errorCode;
        }

        Error AccountManager::AddLevel(const std::string &str_level_info, std::string &out_str_reply)
        {
            Error errorCode(INTERNAL_SERVER_ERROR); 

            if(!IsLevelNameVaild(str_level_info, out_str_reply)){

                out_str_reply = m_error_reply.GetError(out_str_reply, "<AccountManager::AddLevel> FORBIDDEN");
                return FORBIDDEN;
            }

            LOGGER_S(info) << "AddLevel" << str_level_info;

            auto level = std::make_shared<Level>(m_sp_account_db, str_level_info);
            {            
                std::lock_guard<std::mutex> autoLock(m_mux_levels);
                std::string levelName = level->LevelName();

                int sql_error;
                if(!level->AddLevel(sql_error)){
                     
                    if((SQLError)sql_error == CONSTRAINT){
                        out_str_reply = m_error_reply.GetError("DataBase Have This LevelName", "<AccountManager::AddLevel> FORBIDDEN");
                        errorCode = FORBIDDEN;
                    }
                    else
                        out_str_reply = m_error_reply.GetError("Insert To Database Error","<AccountManager::AddLevel> INTERNAL_SERVER_ERROR");
                    
                    
                    return errorCode;      
                }

                m_map_levels.insert(std::pair<std::string, std::shared_ptr<Level> >(levelName, level));
                out_str_reply = level->JsonString();
                errorCode = SUCCESS_RESPONSE;           
            }
            
            return errorCode;
        }

        Error AccountManager::GetLevel(const std::string &str_level_name, std::string &out_str_reply)
        {
            Error errorCode(INTERNAL_SERVER_ERROR); 
            std::lock_guard<std::mutex> autoLock(m_mux_levels);

            auto it = m_map_levels.find(str_level_name.c_str());
            if (it != m_map_levels.end()){

                errorCode = SUCCESS_RESPONSE;
                out_str_reply = it->second->JsonString();
            }
            else{
                
                errorCode = NAME_NOT_FOUND; 
                out_str_reply = m_error_reply.GetError("Level Name Not Found", "<AccountManager::GetLevel> NAME_NOT_FOUND");
            }
                 
            return errorCode;
        }

        Error AccountManager::UpdateLevel(const std::string &str_level_name, const std::string &str_level_info,
                std::string &out_str_reply)
        {
            Error errorCode(INTERNAL_SERVER_ERROR); 
            std::unique_lock<std::mutex> uniquelock(m_mux_levels);

            auto it = m_map_levels.find(str_level_name.c_str());
            if (it != m_map_levels.end()){

                json info = json::parse(str_level_info); 
                std::string level;

                /* check have level name to change */
                if(IsKeyExsist(info, "levelName", level)){

                   /* check level name have be using */ 
                   uniquelock.unlock();
                   if(!IsLevelNameBeUsing(str_level_name)){

                       uniquelock.lock();

                       /* if account change reset map */   
                       if(level != str_level_name){   

                           /* check userinfo vaild */   
                          if(!IsLevelNameVaild(str_level_info, out_str_reply)){

                               uniquelock.unlock();
                               return FORBIDDEN;
                          }
                          
                          out_str_reply = it->second->UpdateLevel(str_level_info); 
                          m_map_levels.insert(std::pair<std::string, std::shared_ptr<Level> >(it->second->LevelName(), it->second));
                          m_map_levels.erase(it);
                       }  
                   }
                   else{

                       errorCode = FORBIDDEN; 
                       out_str_reply = m_error_reply.GetError("Level Name Be Using", "<AccountManager::UpdateLevel> FORBIDDEN");
                       return errorCode;    
                   }
                }
                else
                    out_str_reply = it->second->UpdateLevel(str_level_info);

                errorCode = SUCCESS_RESPONSE;
            }
            else{
                
                errorCode = NAME_NOT_FOUND; 
                out_str_reply = m_error_reply.GetError("Level Name Not Found", "<AccountManager::UpdateLevel> NAME_NOT_FOUND");
            }
                 
            return errorCode;    
        }

        Error AccountManager::DeleteLevel(const std::string &str_level_name, std::string &out_str_reply)
        {
            Error errorCode(INTERNAL_SERVER_ERROR); 
            std::unique_lock<std::mutex> uniquelock(m_mux_levels);

            auto it = m_map_levels.find(str_level_name.c_str());
            if (it != m_map_levels.end()){

                uniquelock.unlock();
                if(!IsLevelNameBeUsing(str_level_name)){

                    uniquelock.lock();
                    it->second->DeleteLevel();
                    m_map_levels.erase(it);
                    errorCode = PET_DELETE;
                }
                else{
       
                    errorCode = FORBIDDEN;
                    out_str_reply = m_error_reply.GetError("Level Name Be Using", "<AccountManager::UpdateLevel> FORBIDDEN");
                }
            }
            else{
  
                errorCode = NAME_NOT_FOUND; 
                out_str_reply = m_error_reply.GetError("Level Name Not Found", "<AccountManager::UpdateLevel> NAME_NOT_FOUND");
            }
                 
            return errorCode;  
        }

        void AccountManager::CreateUsersInfo()
        {
            std::list<std::string> lst_json_user_info;
            m_sp_account_db->GetAllItems(AccountDB::USERS, lst_json_user_info);

            for(auto& it : lst_json_user_info){
                auto user = std::make_shared<User>(m_sp_account_db, it);
                std::string account = user->Account();
                m_map_users.insert(std::pair<std::string, std::shared_ptr<User> >(account, user)); 
            }  
        }

        void AccountManager::CreateLevelsInfo()
        {
            std::list<std::string> lst_json_level_info;
            m_sp_account_db->GetAllItems(AccountDB::LEVELS, lst_json_level_info);

            for(auto& it : lst_json_level_info){

                auto level = std::make_shared<Level>(m_sp_account_db, it);
                std::string level_name = level->LevelName(); 
                m_map_levels.insert(std::pair<std::string, std::shared_ptr<Level> >(level_name, level)); 
            } 
        }

        bool AccountManager::IsAccountVaild(const std::string &str_user_info, std::string &out_str_reply)
        {
            bool vaild(true);

            std::string error_msg;
            std::string account;
            json user_info = json::parse(str_user_info);
            IsKeyExsist(user_info, "account", account);

            LOGGER_S(info) << account;
           
            if (!boost::regex_match (account, boost::regex("^[\x21-\x7F]*$"))){
                
                LOGGER_S(debug) << "User Account Invaild";
                out_str_reply += "Account Invaild. ";
                vaild = false;
            }

            return vaild;
        }

        bool AccountManager::IsPasswordVaild(const std::string &str_user_info, std::string &out_str_reply)
        {
            bool vaild(true);

            std::string error_msg;
            std::string password;
            json user_info = json::parse(str_user_info);
            IsKeyExsist(user_info, "password", password);

            LOGGER_S(info) << password;
           
            if (!boost::regex_match (password, boost::regex("^[\x20-\x7F]*$"))){
                
                LOGGER_S(debug) << "User Password Invaild";
                out_str_reply += "Password Invaild. ";
                vaild = false;
            }

            return vaild;
        }

        bool AccountManager::IsLevelNameVaild(const std::string &str_user_info, std::string &out_str_reply)
        {
            bool vaild(true);

            std::string level;
            json user_info = json::parse(str_user_info);
            IsKeyExsist(user_info, "levelName", level);

            LOGGER_S(info) << level;
           
            if (!boost::regex_match (level, boost::regex("^[\x21-\x7F]*$"))){
                
                LOGGER_S(debug) << "User LevelName Invaild";
                out_str_reply += "LevelName Invaild. ";
                vaild = false;
            }
            
            return vaild;
        }

        bool AccountManager::IsUserInfoVaild(const std::string &str_user_info, std::string &out_str_reply)
        {
            bool vaild(true);

            if(!IsAccountVaild(str_user_info, out_str_reply))
                vaild = false;
            if(!IsPasswordVaild(str_user_info, out_str_reply))
                vaild = false;
            if(!IsLevelNameVaild(str_user_info, out_str_reply))
                vaild = false;
            
            return vaild;
            
        }

        bool AccountManager::IsLevelNameExist(const std::string &str_user_info)
        {
            std::string levelName;
            json user_info = json::parse(str_user_info);

            /* web reply no levelName, so use exsit levelName to do */
            if(!IsKeyExsist(user_info, "levelName", levelName)) 
                return true;
            
            std::lock_guard<std::mutex> autoLock(m_mux_levels);
            auto it = m_map_levels.find(levelName.c_str());
            if (it != m_map_levels.end())
                return true;
            
            return false;
        }

        bool AccountManager::IsLevelNameBeUsing(const std::string &str_level_name)
        {
            std::lock_guard<std::mutex> autoLock(m_mux_users);
            for(auto& it : m_map_users){

                if(str_level_name == it.second->Level())
                    return true;
            }

            return false;
        }
    }
}
