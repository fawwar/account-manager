#pragma once

#include <memory>
#include <string>
#include <list>

#include "AccountDefine.h"

using json = nlohmann::json;

namespace gorilla {
    namespace account{

        class Level
        {
        public:
            
            Level(const std::string& str_level_info);

            ~Level();

            std::string LevelName() const;

            json Json() const;
        
            std::string JsonString() const;

            bool AddLevel(int& out_n_sql_error);

            std::string Features(const std::list<std::string>& lst_fields) const;

            std::string UpdateLevel(const std::string &str_level_info);

            bool DeleteLevel();
            
        private:

            json m_json_level_info;

            std::string m_str_levelName;
            std::string m_str_features;
            std::string m_str_description;

            void SetLevelInfo(const std::string& str_level_info);
        };
    }
}

