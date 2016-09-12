#include <list>
#include <vector>
#include <fstream>
#include <boost/filesystem.hpp>

#include "AccountDefine.h"
#include "hash_codec.hpp"
#include "AccountDB.h"
#include "sqlite3.h"
#include "gorilla/log/logger.h"
#include "gorilla/log/logger_config.h"


static const std::string DB_PATH = "/opt/ivar/var/";
static const std::string DB_NAME = "account.db";
static const std::vector<std::string> TABLE_NAME = {"users", "levels"};
static const int SQL_CMD_LEN = 1024;

std::mutex gorilla::account::AccountDB::g_mutexInstance;

using json = nlohmann::json;
using namespace gorilla::log;

namespace gorilla {
    namespace account{

        AccountDB::AccountDB()
            : m_pSQLDB(nullptr)
            , m_strDBFilePath(DB_PATH + DB_NAME)
        {
            if(boost::filesystem::exists(DB_PATH) == false)
                boost::filesystem::create_directories(DB_PATH);
          
            if(boost::filesystem::exists(m_strDBFilePath) == false){
                std::fstream fp;
                fp.open(m_strDBFilePath, std::ios::out);
                fp.close();
            }

            OpenDB();
        }
            
        AccountDB::~AccountDB()
        {
            if(!m_pSQLDB)
                CloseDB(m_pSQLDB);
        }

        std::shared_ptr<AccountDB>& AccountDB::Instance()
        {
            static std::shared_ptr<AccountDB> instance(nullptr);
         
            if (!instance){
                
                std::lock_guard<std::mutex> lock(g_mutexInstance);
                if (!instance) 
                    instance.reset(new AccountDB());
            }
            
            return instance;
        }

        bool AccountDB::GetAllItems(TableName e_table_name, std::list<std::string>& out_lst_json_string)
        {
            char szSQLCommand[SQL_CMD_LEN];
            sprintf(szSQLCommand, "select * from %s", TABLE_NAME[e_table_name].c_str());
            
            char** query_result;
            char* pErrMsg(nullptr);
            int row(0);
            int col(0);
            {
                std::lock_guard<std::mutex> autoLock(m_muxSQLDB);
                if (sqlite3_get_table(m_pSQLDB, szSQLCommand, &query_result, &row, &col, &pErrMsg) != SQLITE_OK){
                    
                    LOGGER_S(fatal) << "Get Database Error: "<< szSQLCommand <<":" << pErrMsg;
                    sqlite3_free(pErrMsg);
                    return false;
                }
            }
            
            LOGGER_S(info) << "Read Table Name: "<< TABLE_NAME[e_table_name].c_str() <<":" << row << " Items\n";
            
            int index = col;
            for(int row_id = 0; row_id < row; ++row_id){

                char json_string[4096];
                char *p_json_string = &json_string[0];
                
                p_json_string += sprintf(p_json_string, "{");
                
                for(int col_id = 0 ; col_id < col ; ++col_id){

                    char* key = query_result[col_id];
                    char* val = query_result[index];

                    p_json_string += sprintf(p_json_string, "\"%s\":%s,", key, val);

                    //LOGGER_S(debug) << "key = " << key;
                    //LOGGER_S(debug) << "val = " << val;
                    //LOGGER_S(debug) << "json_string = " << json_string;
                    
                    ++index;
                }

                p_json_string -= sizeof(char);
                p_json_string += sprintf(p_json_string, "}");
                out_lst_json_string.push_back(json_string);

            }

            
            return true;
        }

        bool AccountDB::Insert(TableName e_table_name, const json& json_obj, int& out_n_sql_error)
        {
            char szSQLCommand[SQL_CMD_LEN];
            char *pSQLCommand = &szSQLCommand[0];

            pSQLCommand += sprintf(pSQLCommand, "insert into %s", TABLE_NAME[e_table_name].c_str());
            SetSQLInsertTableCmd(json_obj, INSERT_FIELD ,pSQLCommand); 

            pSQLCommand += sprintf(pSQLCommand, "values");
            SetSQLInsertTableCmd(json_obj, INSERT_VALUE , pSQLCommand);

            LOGGER_S(debug) << "Insert "<< TABLE_NAME[e_table_name].c_str() <<" SQL Command >\n" << szSQLCommand;

            char* pErrMsg = NULL;
            {
                std::lock_guard<std::mutex> autoLock(m_muxSQLDB);
                out_n_sql_error = sqlite3_exec(m_pSQLDB, szSQLCommand, NULL, NULL, &pErrMsg);
                if (out_n_sql_error!= SQLITE_OK){

                    LOGGER_S(fatal) << "Insert Database Error: "<< szSQLCommand <<":" << pErrMsg;
                    sqlite3_free(pErrMsg);
                    return false;
                }
            }

            return true;
        }

        bool AccountDB::Update(TableName e_table_name, const std::string& str_update_key_field,
                 const std::string& str_update_key_value, const json& json_obj)
        {
            char szSQLCommand[SQL_CMD_LEN];
            char *pSQLCommand = &szSQLCommand[0];
            pSQLCommand += sprintf(pSQLCommand, "update %s set ", TABLE_NAME[e_table_name].c_str());
            SetSQLUpdatetTableCmd(json_obj, INSERT_FIELD ,pSQLCommand); 
            

            pSQLCommand += sprintf(pSQLCommand, " where %s = '\"%s\"'",
                str_update_key_field.c_str(), str_update_key_value.c_str());
           
            
            
            char* pErrMsg = NULL;
            {
                std::lock_guard<std::mutex> autoLock(m_muxSQLDB);        
                if (sqlite3_exec(m_pSQLDB, szSQLCommand, NULL, NULL, &pErrMsg) != SQLITE_OK){
                
                    LOGGER_S(fatal) << "Update Database Error: "<< szSQLCommand <<":" << pErrMsg;
                    sqlite3_free(pErrMsg);
                    return false;
                }
            }
            
            return true;
        }

        bool AccountDB::Delete(TableName e_table_name, const std::string& str_delete_key_field,
                 const std::string& str_delete_key_value)
        {
            char szSQLCommand[SQL_CMD_LEN];
            sprintf(szSQLCommand, "delete from %s where %s = '\"%s\"'", 
                TABLE_NAME[e_table_name].c_str(), str_delete_key_field.c_str(), str_delete_key_value.c_str());

            char* pErrMsg = NULL;
            {
                std::lock_guard<std::mutex> autoLock(m_muxSQLDB);        
                if (sqlite3_exec(m_pSQLDB, szSQLCommand, NULL, NULL, &pErrMsg) != SQLITE_OK){
                
                    LOGGER_S(fatal) << "Delete Database Error: "<< szSQLCommand <<":" << pErrMsg;
                    sqlite3_free(pErrMsg);
                    return false;
                }
            }
            
            return true;
        }

        bool AccountDB::OpenDB()
        { 
            if (sqlite3_open_v2(m_strDBFilePath.c_str(), &m_pSQLDB, SQLITE_OPEN_READWRITE, NULL) != SQLITE_OK){
                
                LOGGER_S(error) << "Open Database Error: " << m_strDBFilePath.c_str() << ":" << sqlite3_errmsg(m_pSQLDB); 
                return false;
            }

            for(const auto &v_table : TABLE_NAME)
                CreateTable(m_pSQLDB, v_table);

            LOGGER_S(info) << "Database Open Successfully \n";
            return true;
        }
            
        void AccountDB::CloseDB(SQLiteDB* pSQLiteDB)
        {
            sqlite3_close(m_pSQLDB);
        }

        void AccountDB::CreateTable(SQLiteDB* p_SQLDB, const std::string& str_table_name)
        {
            char szSQLCommand[SQL_CMD_LEN];
            char *pSQLCommand = &szSQLCommand[0];

            pSQLCommand += sprintf(pSQLCommand, "create table if not exists %s", str_table_name.c_str());

            if(str_table_name == "users"){
                
               std::string hash_pass = hash::TextEncodeHash("gorilla");
               
               pSQLCommand += sprintf(pSQLCommand, "(account char(32) not null primary key,");
               pSQLCommand += sprintf(pSQLCommand, "encryptedPassword char(128) null,");
               pSQLCommand += sprintf(pSQLCommand, "levelName char(16) null,");
               pSQLCommand += sprintf(pSQLCommand, "description text '""');");
               pSQLCommand += sprintf(pSQLCommand, "insert or ignore into users ");
               pSQLCommand += sprintf(pSQLCommand, "(account,encryptedPassword,levelName,description) ");
               pSQLCommand += sprintf(pSQLCommand, "values('\"admin\"','\"%s\"','\"admin\"','\"\"');", hash_pass.c_str());
            }
            else if(str_table_name == "levels"){

               json j_features(DEFAULT_FEATURES); 
               std::string features = j_features.dump();
                
               pSQLCommand += sprintf(pSQLCommand, "(levelName char(32) not null primary key,");
               pSQLCommand += sprintf(pSQLCommand, "features char(32) null,");
               pSQLCommand += sprintf(pSQLCommand, "description text '""');");
               pSQLCommand += sprintf(pSQLCommand, "insert or ignore into levels(levelName,features,description) ");
               pSQLCommand += sprintf(pSQLCommand, "values('\"admin\"','%s','\"\"');", features.c_str());
            }

            char* pErrMsg = NULL;
            if (sqlite3_exec(m_pSQLDB, szSQLCommand, NULL, NULL, &pErrMsg) != SQLITE_OK) 
            {
               LOGGER_S(fatal) << "Create Database Table Error: "<< szSQLCommand <<":" << pErrMsg;
               sqlite3_free(pErrMsg);
            }

            auto it = m_mapDBTable.find(str_table_name);
            if (it == m_mapDBTable.end())
               m_mapDBTable.insert(std::pair<std::string, SQLiteDB* >(str_table_name.c_str(), m_pSQLDB));
        }

        
        void AccountDB::SetSQLInsertTableCmd(const json& json_obj, CommandType e_cmd_type,
            char*& out_cmd)
        {
            out_cmd += sprintf(out_cmd, "(");
            for(auto it = json_obj.begin(); it != json_obj.end(); ++it){
        
                if(e_cmd_type == INSERT_FIELD){
                    
                    LOGGER_S(info) << "Set SQL Key = " << it.key();
                    out_cmd += sprintf(out_cmd, "%s,", it.key().c_str());
                }
                else{
          
                    std::string str_val = it.value().dump();      
                    LOGGER_S(info) << "Set SQL Value = " << str_val;
                    out_cmd += sprintf(out_cmd, "'%s',", str_val.c_str());
                    
                }
            }
        
            out_cmd -= sizeof(char);
            out_cmd += sprintf(out_cmd, ")");
        }

        void AccountDB::SetSQLUpdatetTableCmd(const json& json_obj, CommandType e_cmd_type,
                char*& out_cmd)
        {
            for(auto it = json_obj.begin(); it != json_obj.end(); ++it){

                std::string str_val = it.value().dump();
            
                out_cmd += sprintf(out_cmd, "%s='%s',", 
                    it.key().c_str(), str_val.c_str());  
            }
            
            out_cmd -= sizeof(char);
        }
    
    }
}

