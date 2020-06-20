#pragma once

#include <string>
#include <memory>
#include <mutex>
#include <map>
#include <list>

#include "./import/json-develop/json.hpp"
#define DB_INSTANCE AccountDB::Instance()

// Forward declaration
struct sqlite3;
typedef struct sqlite3 SQLiteDB;
using json = nlohmann::json;

namespace gorilla {
    namespace account{

        class AccountDB
        {
        public:

            enum TableName 
            {
                USERS,
                LEVELS
            };
       
            static AccountDB& Instance();
            
            bool GetAllItems(TableName e_table_name, std::list<std::string>& out_lst_json_string);

            bool Insert(TableName e_table_name, const json& json_obj, int& out_n_sql_error);

            bool Update(TableName e_table_name, const std::string& str_update_key_field,
                 const std::string& str_update_key_value, const json& json_obj);

            bool Delete(TableName e_table_name, const std::string& str_delete_key_field,
                 const std::string& str_delete_key_value);

        private:

            enum CommandType{INSERT_FIELD, INSERT_VALUE};
            
            const std::string m_strDBFilePath;
            static std::mutex g_mutexInstance;
            mutable std::mutex m_muxSQLDB;
            SQLiteDB* m_pSQLDB;
            std::map<std::string, SQLiteDB*> m_mapDBTable;

            AccountDB();

            ~AccountDB();

            AccountDB(const AccountDB& rhs);

            bool OpenDB();
            
            void CloseDB();
            
            void AdjustTable(const std::string& str_table_name);
            
            void CreateTable(const std::string& str_table_name);

            void SetSQLInsertTableCmd(const json& json_obj, CommandType e_cmd_type,
                char*& out_cmd);

            void SetSQLUpdatetTableCmd(const json& json_obj, CommandType e_cmd_type,
                char*& out_cmd);

	         
        };
    }
}

