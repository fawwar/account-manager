#include "AccessRightInfo.h"
#include "AccountDB.h"
#include "hash_codec.hpp"

#include "./import/json-develop/json_tools.hpp"
#include "gorilla/log/logger.h"
#include "gorilla/log/logger_config.h"


using namespace gorilla::log;
namespace
{
    const std::string UPDATE_KEY_FIELD = "accessRightName";
}


namespace gorilla {
    namespace account{

        AccessRight::AccessRight(const std::string& str_access_right_info)
            : m_json_access_right_info(DISABLE_ACCESSRIGHT)
        { 
            SetAccessRightInfo(str_access_right_info);    
        }

        AccessRight::~AccessRight()
        {
        }

        std::string AccessRight::AccessRightName() const
        {
            return m_str_access_right_name;
        }

        json AccessRight::Json() const
        {
            return m_json_access_right_info;
        }

        std::string AccessRight::JsonString() const
        {
            return Json().dump();
        }

        bool AccessRight::AddAccessRight(int& out_n_sql_error)
        {
            return DB_INSTANCE.Insert(AccountDB::LEVELS, m_json_access_right_info, out_n_sql_error);
        }

        std::string AccessRight::Permissions(const std::list<std::string>& lst_fields) const
        {
            json j_permissions = m_json_access_right_info["permissions"];
            json j_out_permissions;

            if(lst_fields.empty())
                return JsonString();
            
            //LOGGER_S(info) << "j_features = " << j_features;
            
            for(auto& it : lst_fields){

                std::string fields(it);
                std::string field_1(it);
                std::string field_2;
                    
                std::size_t pos = it.find('.');    
                if (pos != std::string::npos){

                    field_1 = fields.substr(0, pos);
                    field_2 = fields.substr(pos + 1);

                    json j_val_1;
                    if(IsKeyExsist(j_permissions, field_1, j_val_1)){

                        json j_val_2;
                        if(IsKeyExsist(j_val_1, field_2, j_val_2)){
                            
                            j_out_permissions[field_1][field_2] = 
                                j_permissions[field_1][field_2];
                        }
                    }
                }
                else{

                    if(IsKeyExsist(j_permissions, field_1))
                        j_out_permissions[field_1] = j_permissions[field_1];
                }           
            }

            return j_out_permissions.dump();
        }

        std::string AccessRight::UpdateAccessRight(const std::string &str_access_right_info)
        {
            json j_access_right_info = json::parse(str_access_right_info); 
            SetJson(j_access_right_info, m_json_access_right_info);   
            DB_INSTANCE.Update(AccountDB::LEVELS, UPDATE_KEY_FIELD,
                m_str_access_right_name, m_json_access_right_info);

            SetAccessRightInfo(m_json_access_right_info.dump());

            return JsonString();
        }

        bool AccessRight::DeleteAccessRight()
        {
            return DB_INSTANCE.Delete(AccountDB::LEVELS, UPDATE_KEY_FIELD, m_str_access_right_name);    
        }

        void AccessRight::SetAccessRightInfo(const std::string& str_access_right_info)
        {   
            json j_level_info = json::parse(str_access_right_info);
            SetJson(j_level_info, m_json_access_right_info);

            m_str_access_right_name = m_json_access_right_info["accessRightName"];
            m_str_permissions = m_json_access_right_info["permissions"].dump();
            if(!IsKeyExsist(m_json_access_right_info, "description", m_str_description)){
               m_json_access_right_info["description"] = " "; 
            }
          

            //LOGGER_S(info) << "m_str_access_right_name= " << m_str_access_right_name;
            //LOGGER_S(info) << "m_str_permissions= " << m_str_permissions;
            //LOGGER_S(info) << "m_json_access_right_info= " << m_json_access_right_info;
        }
    }
}

