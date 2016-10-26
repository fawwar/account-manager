#pragma once

#include <memory>
#include <string>
#include <list>

#include "AccountDefine.h"

using json = nlohmann::json;

namespace gorilla {
    namespace account{

        class AccessRight
        {
        public:
            
            AccessRight(const std::string& str_access_right_info);

            ~AccessRight();

            std::string AccessRightName() const;

            json Json() const;
        
            std::string JsonString() const;

            bool AddAccessRight(int& out_n_sql_error);

            std::string Permissions(const std::list<std::string>& lst_fields) const;

            std::string UpdateAccessRight(const std::string &str_access_right_info);

            bool DeleteAccessRight();
            
        private:

            json m_json_access_right_info;

            std::string m_str_access_right_name;
            std::string m_str_permissions;
            std::string m_str_description;

            void SetAccessRightInfo(const std::string& str_access_right_info);
        };
    }
}

