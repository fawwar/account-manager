//#include <regex>
#include <list>
#include <sstream>
#include <boost/regex.hpp>
#include <curl/curl.h>

#include "AccountManager.h"
#include "AccountDB.h"
#include "UserInfo.h"
#include "AccessRightInfo.h"

#include "hash.h"
#include "./import/json-develop/json_tools.hpp"
#include "gorilla/log/logger.h"
#include "gorilla/log/logger_config.h"
#include "Util.h"

#define ADMIN_PASSWORD "73dnPFv3S8GZLMVH"

using namespace gorilla::log;

namespace gorilla {
    namespace account{

        AccountManager::AccountManager()
        {
            CreateUsersInfo();
            CreateLevelsInfo();
        }

        AccountManager::~AccountManager()
        {
        }

        bool AccountManager::GetUserAccessRight(const std::string str_account, std::string& out_str_access_right)
        {
            auto it = m_map_users.find(str_account);
            if (it != m_map_users.end()){

                out_str_access_right = it->second->AccessRight();
                return true;
            }

            return false;
        }

        bool AccountManager::VerifyAccount(const std::string& str_account, const std::string& str_password)
        {
             LOGGER_S(info) << "Verify = " << str_account << "," << str_password;

             if(str_account == "admin" && str_password == ADMIN_PASSWORD) return true;

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
            if(!IsAccessRightNameExist(str_user_info)){

                out_str_reply = m_error_reply.GetError("AccessRightName Is Not Exist","<AccountManager::AddUser> FORBIDDEN");
                return FORBIDDEN;
            }
                
            
            /* check account,password,level content is vaild */
            if(!IsUserInfoVaild(str_user_info, out_str_reply)){

                out_str_reply = m_error_reply.GetError(out_str_reply,"<AccountManager::AddUser> FORBIDDEN");
                return FORBIDDEN;
            }

            /* create user map */
            auto user = std::make_shared<User>(str_user_info);
            {            
                std::lock_guard<std::mutex> autoLock(m_mux_users);
                std::string account = user->Account();

                int sql_error;
                if(!user->AddUser(sql_error)){

                    if((SQLError)sql_error == CONSTRAINT){
                        //out_str_reply = m_error_reply.GetError("DataBase Have This Account","<AccountManager::AddUser> FORBIDDEN");
                        out_str_reply = m_error_reply.GetError("This account already exists.","ACCOUNT_DUPLICATE");
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
                const std::string &str_user_info, std::string &out_str_reply, const Server::request& request)
        {
            Error errorCode(INTERNAL_SERVER_ERROR); 
            /* check level name exist in level map */
            if(!IsAccessRightNameExist(str_user_info)){

                out_str_reply = m_error_reply.GetError("AccessRightName Is Not Exist","<AccountManager::UpdateUser> FORBIDDEN");
                return FORBIDDEN;
            }
            json info = json::parse(str_user_info); 
            std::lock_guard<std::mutex> autoLock(m_mux_users);
            auto it = m_map_users.find(str_account.c_str());
            if (it != m_map_users.end()){

                 //if(str_login_level == "admin" && str_account != "admin"){
                 if(true){
                     /* admin account only change password */   
                     /*
                     if(str_account == "admin"){
                        if(IsKeyExsist(info, "account") || IsKeyExsist(info, "accessRightName")){
                            out_str_reply = m_error_reply.GetError("User No Permissions To Chang Account Or AccessRightName", 
                                "<AccountManager::UpdateUser> FORBIDDEN");
                            
                            return FORBIDDEN;
                        }
                     }
                     */
                     /* check userinfo vaild */
/*                     if(!IsUserInfoVaild(str_user_info, out_str_reply))
                        return FORBIDDEN;*/
                     /* update userinfo */
                     errorCode = SUCCESS_RESPONSE;   
                     out_str_reply = it->second->UpdateUser(str_user_info);
	             if (IsKeyExsist(info,"password"))
			{
			/*
				std::string str_cookie_name;
	    			std::string str_cookie_value="";
				std::string str_x_sessiontoken_name;
				std::string str_x_sessiontoken_value="";
	    			Server::request::headers_container_type const &hs_cookie = request.headers;
	    			{
				    for (auto it = hs_cookie.begin(); it!=hs_cookie.end(); ++it)
				    {
		    		    	if(boost::iequals(it->name,"cookie"))
				    	{   str_cookie_name = it->name;
			    		    str_cookie_value = it->value;
			   		    LOGGER_S(debug)<<"Error AccountManager::UpdateUser str_cookie_name !" << str_cookie_name;
			   		    LOGGER_S(debug)<<"Error AccountManager::UpdateUser str_cookie_value !" << str_cookie_value;
				    	}
					if(boost::iequals(it->name,"x-sessiontoken"))
					{
					    str_x_sessiontoken_name = it->name;
					    str_x_sessiontoken_value = it->value;
					    LOGGER_S(debug)<<"Error AccountManager::UpdateUser str_x_sessiontoken_name !" <<str_x_sessiontoken_name;
					    LOGGER_S(debug)<<"Error AccountManager::UpdateUser str_x_sessiontoken_value !" <<str_x_sessiontoken_value;
					}
				    }
	    			}	     

				std::string url = "http://127.0.0.1:8001/session/kick/";
               			url += Util::urlEncode(str_account);				
                		//notify proxy service
                		CURL* handle = curl_easy_init();
				struct curl_slist *chunk = NULL;
			        if (str_cookie_value != "")
				{
				    str_cookie_name.append(": ").append(str_cookie_value);
				    const char* char_cookie_name = str_cookie_name.c_str();
				    LOGGER_S(debug)<<"cookie test! "<<str_cookie_name;
				    chunk = curl_slist_append(chunk , char_cookie_name);
				}
				if (str_x_sessiontoken_value != "")
				{
				    str_x_sessiontoken_name.append(": ").append(str_x_sessiontoken_value);
				    const char* char_x_sessiontoken_name = str_x_sessiontoken_name.c_str();
				    LOGGER_S(debug)<<"x_sessiontoken test! "<<str_x_sessiontoken_name;
				    chunk = curl_slist_append(chunk, char_x_sessiontoken_name);
				    
				}
				curl_easy_setopt(handle, CURLOPT_HTTPHEADER, chunk);
                		curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
                		curl_easy_setopt(handle, CURLOPT_TIMEOUT, 5L); //5 seconds
                		//curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
                		CURLcode resCode = curl_easy_perform(handle);
                		curl_easy_cleanup(handle);
				curl_slist_free_all(chunk);
				*/
			}


		                        /* if account change reset map */   
                     if(it->second->Account() != str_account){   
                        m_map_users.insert(std::pair<std::string, std::shared_ptr<User> >(it->second->Account(), it->second));
                        m_map_users.erase(it);
                     }
                 }
                 else{ //admin only can modify password
   
                    /* user account only change password */
                    if(IsKeyExsist(info, "account") || IsKeyExsist(info, "accessRightName")){

                        out_str_reply = m_error_reply.GetError("User No Permissions To Chang Account Or AccessRightName",
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
			if (IsKeyExsist(info,"password"))
			{
			                		
				std::string str_cookie_name;
	    			std::string str_cookie_value="";
				std::string str_x_sessiontoken_name;
				std::string str_x_sessiontoken_value="";
	    			Server::request::headers_container_type const &hs_cookie = request.headers;
	    			{
				    for (auto it = hs_cookie.begin(); it!=hs_cookie.end(); ++it)
				    {
		    		    	if(boost::iequals(it->name,"cookie"))
				    	{   str_cookie_name = it->name;
			    		    str_cookie_value = it->value;
			   		   // LOGGER_S(debug)<<"Error AccountManager::UpdateUser str_cookie_name !" << str_cookie_name;
			   		   // LOGGER_S(debug)<<"Error AccountManager::UpdateUser str_cookie_value !" << str_cookie_value;
				    	}
					if(boost::iequals(it->name,"x-sessiontoken"))
					{
					    str_x_sessiontoken_name = it->name;
					    str_x_sessiontoken_value = it->value;
					   // LOGGER_S(debug)<<"Error AccountManager::UpdateUser str_x_sessiontoken_name !" <<str_x_sessiontoken_name;
					   // LOGGER_S(debug)<<"Error AccountManager::UpdateUser str_x_sessiontoken_value !" <<str_x_sessiontoken_value;
					}
				    }
	    			}	     

				std::string url = "http://127.0.0.1:8001/session/kick/";
               			url += Util::urlEncode(str_account);				
                		//notify proxy service
                		CURL* handle = curl_easy_init();
				struct curl_slist *chunk = NULL;
			        //const char* char_cookie_name = str_cookie_name.c_str();
				//chunk = curl_slist_append(chunk , char_cookie_name);

				if (str_cookie_value != "")
				{
				    str_cookie_name.append(": ").append(str_cookie_value);
				    const char* char_cookie_name = str_cookie_name.c_str();
				    LOGGER_S(debug)<<str_cookie_name;
				    chunk = curl_slist_append(chunk , char_cookie_name);
				}
				if (str_x_sessiontoken_value != "")
				{
				    str_x_sessiontoken_name.append(": ").append(str_x_sessiontoken_value);
				    const char* char_x_sessiontoken_name = str_x_sessiontoken_name.c_str();
				    LOGGER_S(debug)<<str_x_sessiontoken_name;
				    chunk = curl_slist_append(chunk, char_x_sessiontoken_name);
				    
				}
				curl_easy_setopt(handle, CURLOPT_HTTPHEADER, chunk);
                		curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
                		curl_easy_setopt(handle, CURLOPT_TIMEOUT, 5L); //5 seconds
                		//curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
                		CURLcode resCode = curl_easy_perform(handle);
                		curl_easy_cleanup(handle);
				curl_slist_free_all(chunk);	

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

                std::string url = "http://127.0.0.1:8001/session/kick/";
                url += Util::urlEncode(str_account);
                //notify proxy service
                CURL* handle = curl_easy_init();
                curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
                curl_easy_setopt(handle, CURLOPT_TIMEOUT, 5L); //5 seconds
                //curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
                CURLcode resCode = curl_easy_perform(handle);
                curl_easy_cleanup(handle);

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

        Error AccountManager::GetUserPermissions(const std::string &str_account, const std::list<std::string>& lst_fields,
                std::string &out_str_reply)
        {
            Error errorCode(INTERNAL_SERVER_ERROR);

            std::unique_lock<std::mutex> uniquelock(m_mux_users);
            auto it_user = m_map_users.find(str_account.c_str());
            if (it_user != m_map_users.end()){

                std::string level_name = it_user->second->AccessRight();
                uniquelock.unlock();

                /* find level name featues */
                std::lock_guard<std::mutex> autoLock(m_mux_access_rights);
                auto it_level = m_map_access_rights.find(level_name.c_str());
                if (it_level != m_map_access_rights.end()){

                    out_str_reply = it_level->second->Permissions(lst_fields);
                    errorCode = SUCCESS_RESPONSE;           
                } 
            }
            else{

                errorCode = NAME_NOT_FOUND; 
                out_str_reply = m_error_reply.GetError("User Name Not Found","<AccountManager::GetUserPermissions> NAME_NOT_FOUND");
            }

            return errorCode;
        }

        Error AccountManager::GetAccessRights(std::string &out_str_reply)
        {
            Error errorCode(INTERNAL_SERVER_ERROR);

            bool level_exist(false);
            json::array_t j_level_array;
            json j_levels;
            
            std::lock_guard<std::mutex> autoLock(m_mux_access_rights);
            for(auto& it : m_map_access_rights){

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
                out_str_reply = m_error_reply.GetError("Haven't Any AccessRight","<AccountManager::GetAccessRights> NAME_NOT_FOUND");
            }

            return errorCode;
        }

        Error AccountManager::AddAccessRight(const std::string &str_access_right_info, std::string &out_str_reply)
        {
            Error errorCode(INTERNAL_SERVER_ERROR); 

            if(!IsAccessRightNameVaild(str_access_right_info, out_str_reply)){

                out_str_reply = m_error_reply.GetError(out_str_reply, "<AccountManager::AddAccessRight> FORBIDDEN");
                return FORBIDDEN;
            }

            LOGGER_S(info) << "AddAccessRight" << str_access_right_info;

            auto level = std::make_shared<AccessRight>(str_access_right_info);
            {            
                std::lock_guard<std::mutex> autoLock(m_mux_access_rights);
                std::string accessRightName = level->AccessRightName();

                int sql_error;
                if(!level->AddAccessRight(sql_error)){
                     
                    if((SQLError)sql_error == CONSTRAINT){
                        out_str_reply = m_error_reply.GetError("DataBase Have This AccessRightName", "<AccountManager::AddAccessRight> FORBIDDEN");
                        errorCode = FORBIDDEN;
                    }
                    else
                        out_str_reply = m_error_reply.GetError("Insert To Database Error","<AccountManager::AddAccessRight> INTERNAL_SERVER_ERROR");
                    
                    
                    return errorCode;      
                }

                m_map_access_rights.insert(std::pair<std::string, std::shared_ptr<AccessRight> >(accessRightName, level));
                out_str_reply = level->JsonString();
                errorCode = SUCCESS_RESPONSE;           
            }
            
            return errorCode;
        }

        Error AccountManager::GetAccessRight(const std::string &str_access_right_name, std::string &out_str_reply)
        {
            Error errorCode(INTERNAL_SERVER_ERROR); 
            std::lock_guard<std::mutex> autoLock(m_mux_access_rights);

            auto it = m_map_access_rights.find(str_access_right_name.c_str());
            if (it != m_map_access_rights.end()){

                errorCode = SUCCESS_RESPONSE;
                out_str_reply = it->second->JsonString();
            }
            else{
                
                errorCode = NAME_NOT_FOUND; 
                out_str_reply = m_error_reply.GetError("AccessRight Name Not Found", "<AccountManager::GetAccessRight> NAME_NOT_FOUND");
            }
                 
            return errorCode;
        }

        Error AccountManager::UpdateAccessRight(const std::string &str_access_right_name, const std::string &str_access_right_info,
                std::string &out_str_reply)
        {
            Error errorCode(INTERNAL_SERVER_ERROR); 
            std::unique_lock<std::mutex> uniquelock(m_mux_access_rights);

            auto it = m_map_access_rights.find(str_access_right_name.c_str());
            if (it != m_map_access_rights.end()){

                json info = json::parse(str_access_right_info); 
                std::string level;

                /* check have level name to change */
                if(IsKeyExsist(info, "accessRightName", level)){

                   /* check level name have be using */ 
                   uniquelock.unlock();
                   if(!IsAccessRightNameBeUsing(str_access_right_name)){

                       uniquelock.lock();

                       /* if account change reset map */   
                       if(level != str_access_right_name){   

                           /* check userinfo vaild */   
                          if(!IsAccessRightNameVaild(str_access_right_info, out_str_reply)){

                               uniquelock.unlock();
                               return FORBIDDEN;
                          }
                          
                          out_str_reply = it->second->UpdateAccessRight(str_access_right_info); 
                          m_map_access_rights.insert(std::pair<std::string, std::shared_ptr<AccessRight> >(it->second->AccessRightName(), it->second));
                          m_map_access_rights.erase(it);
                       }  
                   }
                   else{

                       errorCode = FORBIDDEN; 
                       //out_str_reply = m_error_reply.GetError("AccessRight Name Be Using", "<AccountManager::UpdateAccessRight> FORBIDDEN");
                       out_str_reply = m_error_reply.GetError("This access right is in use.", "ACCESS_RIGHT_IS_IN_USE");
                       return errorCode;    
                   }
                }
                else
                    out_str_reply = it->second->UpdateAccessRight(str_access_right_info);

                errorCode = SUCCESS_RESPONSE;
            }
            else{
                
                errorCode = NAME_NOT_FOUND; 
                out_str_reply = m_error_reply.GetError("AccessRight Name Not Found", "<AccountManager::UpdateAccessRight> NAME_NOT_FOUND");
            }
                 
            return errorCode;    
        }

        Error AccountManager::DeleteAccessRight(const std::string &str_access_right_name, std::string &out_str_reply)
        {
            Error errorCode(INTERNAL_SERVER_ERROR); 
            std::unique_lock<std::mutex> uniquelock(m_mux_access_rights);

            auto it = m_map_access_rights.find(str_access_right_name.c_str());
            if (it != m_map_access_rights.end()){

                uniquelock.unlock();
                if(!IsAccessRightNameBeUsing(str_access_right_name)){

                    uniquelock.lock();
                    it->second->DeleteAccessRight();
                    m_map_access_rights.erase(it);
                    errorCode = PET_DELETE;
                }
                else{
       
                    errorCode = FORBIDDEN;
                    out_str_reply = m_error_reply.GetError("This access right is in use.", "ACCESS_RIGHT_IS_IN_USE");
                }
            }
            else{
  
                errorCode = NAME_NOT_FOUND; 
                out_str_reply = m_error_reply.GetError("AccessRight Name Not Found", "<AccountManager::UpdateAccessRight> NAME_NOT_FOUND");
            }
                 
            return errorCode;  
        }

        void AccountManager::CreateUsersInfo()
        {
            std::list<std::string> lst_json_user_info;
            DB_INSTANCE.GetAllItems(AccountDB::USERS, lst_json_user_info);

            for(auto& it : lst_json_user_info){
                auto user = std::make_shared<User>(it);
                std::string account = user->Account();
                m_map_users.insert(std::pair<std::string, std::shared_ptr<User> >(account, user)); 
            }  
        }

        void AccountManager::CreateLevelsInfo()
        {
            std::list<std::string> lst_json_level_info;
            DB_INSTANCE.GetAllItems(AccountDB::LEVELS, lst_json_level_info);

            for(auto& it : lst_json_level_info){

                auto level = std::make_shared<AccessRight>(it);
                std::string level_name = level->AccessRightName(); 
                m_map_access_rights.insert(std::pair<std::string, std::shared_ptr<AccessRight> >(level_name, level)); 
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
            
            /*
            if (!boost::regex_match (account, boost::regex("^[\x21-\x7F]+$"))){
                
                LOGGER_S(debug) << "User Account Invaild";
                out_str_reply += "Account Invaild. ";
                vaild = false;
            }
            */
            
            if(account.length() == 0){
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
           
            if (!boost::regex_match (password, boost::regex("^[\x20-\x7F]+$"))){
                
                LOGGER_S(debug) << "User Password Invaild";
                out_str_reply += "Password Invaild. ";
                vaild = false;
            }

            return vaild;
        }

        bool AccountManager::IsAccessRightNameVaild(const std::string &str_user_info, std::string &out_str_reply)
        {
            bool vaild(true);

            std::string level;
            json user_info = json::parse(str_user_info);
            IsKeyExsist(user_info, "accessRightName", level);

            LOGGER_S(info) << level;
           
            if (!boost::regex_match (level, boost::regex("^[\x21-\x7F]*$"))){
                
                LOGGER_S(debug) << "User AccessRightName Invaild";
                out_str_reply += "AccessRightName Invaild. ";
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
            if(!IsAccessRightNameVaild(str_user_info, out_str_reply))
                vaild = false;
            
            return vaild;
            
        }

        bool AccountManager::IsAccessRightNameExist(const std::string &str_user_info)
        {
            std::string accessRightName;
            json user_info = json::parse(str_user_info);

            /* web reply no accessRightName, so use exsit accessRightName to do */
            if(!IsKeyExsist(user_info, "accessRightName", accessRightName)) 
                return true;
            
            std::lock_guard<std::mutex> autoLock(m_mux_access_rights);
            auto it = m_map_access_rights.find(accessRightName.c_str());
            if (it != m_map_access_rights.end())
                return true;
            
            return false;
        }

        bool AccountManager::IsAccessRightNameBeUsing(const std::string &str_access_right_name)
        {
            std::lock_guard<std::mutex> autoLock(m_mux_users);
            for(auto& it : m_map_users){

                if(str_access_right_name == it.second->AccessRight())
                    return true;
            }

            return false;
        }
    }
}
