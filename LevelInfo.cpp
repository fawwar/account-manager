#include "LevelInfo.h"
#include "AccountDB.h"
#include "hash_codec.hpp"

#include "./import/json-develop/json_tools.hpp"
#include "gorilla/log/logger.h"
#include "gorilla/log/logger_config.h"


using namespace gorilla::log;
namespace
{
    const std::string UPDATE_KEY_FIELD = "levelName";
}


namespace gorilla {
    namespace account{

        Level::Level(const std::string& str_level_info)
            : m_json_level_info(DISABLE_LEVEL)
        { 
            SetLevelInfo(str_level_info);    
        }

        Level::~Level()
        {
        }

        std::string Level::LevelName() const
        {
            return m_str_levelName;
        }

        json Level::Json() const
        {
            return m_json_level_info;
        }

        std::string Level::JsonString() const
        {
            return Json().dump();
        }

        bool Level::AddLevel(int& out_n_sql_error)
        {
            return DB_INSTANCE.Insert(AccountDB::LEVELS, m_json_level_info, out_n_sql_error);
        }

        std::string Level::Features(const std::list<std::string>& lst_fields) const
        {
            json j_features = m_json_level_info["features"];
            json j_out_features;

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
                    if(IsKeyExsist(j_features, field_1, j_val_1)){

                        json j_val_2;
                        if(IsKeyExsist(j_val_1, field_2, j_val_2)){
                            
                            j_out_features[field_1][field_2] = 
                                j_features[field_1][field_2];
                        }
                    }
                }
                else{

                    if(IsKeyExsist(j_features, field_1))
                        j_out_features[field_1] = j_features[field_1];
                }           
            }

            return j_out_features.dump();
        }

        std::string Level::UpdateLevel(const std::string &str_level_info)
        {
            json j_level_info = json::parse(str_level_info); 
            SetJson(j_level_info, m_json_level_info);   
            DB_INSTANCE.Update(AccountDB::LEVELS, UPDATE_KEY_FIELD,
                m_str_levelName, m_json_level_info);

            SetLevelInfo(m_json_level_info.dump());

            return JsonString();
        }

        bool Level::DeleteLevel()
        {
            return DB_INSTANCE.Delete(AccountDB::LEVELS, UPDATE_KEY_FIELD, m_str_levelName);    
        }

        void Level::SetLevelInfo(const std::string& str_level_info)
        {   
            json j_level_info = json::parse(str_level_info);
            SetJson(j_level_info, m_json_level_info);

            m_str_levelName = m_json_level_info["levelName"];
            m_str_features = m_json_level_info["features"].dump();
            if(!IsKeyExsist(str_level_info, "description", m_str_description)){
               m_json_level_info["description"] = " "; 
            }
          

            //LOGGER_S(info) << "m_str_levelName= " << m_str_levelName;
            //LOGGER_S(info) << "m_str_features= " << m_str_features;
            //LOGGER_S(info) << "m_json_level_info= " << m_json_level_info;
        }
    }
}

