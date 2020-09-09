#include <list>
#include <vector>
#include <fstream>
#include <boost/filesystem.hpp>

#include "AccountDefine.h"
#include "hash_codec.hpp"
#include "AccountDB.h"
#include "Config.h"
#include "sqlite3.h"
#include "gorilla/log/logger.h"
#include "gorilla/log/logger_config.h"

#include "./import/json-develop/json_tools.hpp"
#include "json/json.h"
#include<string>

#include "LdapConfig.h"

#ifdef WIN32
static const std::string DB_PATH = "./";// "C:/opt/ivar/var/";
#else
static const std::string DB_PATH = "./";
#endif
static const std::string DB_NAME = "account.db";
static const std::vector<std::string> TABLE_NAME = {"users", "accessRights"};
static const int SQL_CMD_LEN = 102400; //20171113 enlarge because we store base64 in access right

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
            CloseDB();
        }

        AccountDB& AccountDB::Instance()
        {
            static AccountDB instance;
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

	int AccountDB::SetInsertCommand(TableName e_table_name, const json& json_obj, char*& out_cmd)
		{
			std::string szSQLCommand_str;
			szSQLCommand_str.append("insert into ");
			szSQLCommand_str.append(TABLE_NAME[e_table_name]);
			std::string json_str = json_obj.dump();
			Json::Reader reader;
			Json::Value info;
			reader.parse(json_str,info);
			szSQLCommand_str.append("(");
			for (auto const&id : info.getMemberNames())
			{
				szSQLCommand_str.append(id);
				szSQLCommand_str.append(",");
			}
			szSQLCommand_str = szSQLCommand_str.replace(szSQLCommand_str.end() - 1, szSQLCommand_str.end(), ")");
			szSQLCommand_str.append("values (");
			for (int i = 0; i < info.size(); i++)
			{
				szSQLCommand_str.append("?,");
			}
			szSQLCommand_str = szSQLCommand_str.replace(szSQLCommand_str.end() - 1, szSQLCommand_str.end(), ")");
			LOGGER_S(debug) << "szSQLCommand_str " << szSQLCommand_str;
			strcpy(out_cmd, szSQLCommand_str.c_str());
			
			return info.size();
		}

        bool AccountDB::Insert(TableName e_table_name, const json& json_obj, int& out_n_sql_error)
        {   
            char szSQLCommand[SQL_CMD_LEN];
            char *pSQLCommand = &szSQLCommand[0];
			
			int count  = SetInsertCommand(e_table_name,json_obj,pSQLCommand);		
			// For the insert and select, we will prepare statements
			sqlite3_stmt *insert_stmt = NULL;
			// SQLite return value
			//int rc;

			out_n_sql_error = sqlite3_prepare_v2(m_pSQLDB, szSQLCommand, -1, &insert_stmt, NULL);

			if (SQLITE_OK != out_n_sql_error) {
				//LOGGER_S(debug) << "Can't prepare insert statment "<< szSQLCommand <<" " << rc << " : " <<sqlite3_errmsg(m_pSQLDB);
				
				throw std::runtime_error("Can't prepare insert statment ");
				//sqlite3_close(m_pSQLDB);
			}
	
			int i = 1;
			for (auto it = json_obj.begin(); it != json_obj.end(); ++it ) {
				
				std::string str_val =  it.value().dump();
				std::string str_key = it.key();
				//LOGGER_S(debug) << "str_key " << str_key;
				//LOGGER_S(debug) << "str_val " << str_val;
				//LOGGER_S(debug) << "str_val.size " << str_val.size();
				
				//The NULL is "Don't attempt to free() the value when it's bound", since it's on the stack here
				
					out_n_sql_error = sqlite3_bind_text(insert_stmt, i, str_val.c_str(), str_val.size(), SQLITE_TRANSIENT);
					if (SQLITE_OK != out_n_sql_error) {
						
						//LOGGER_S(debug) << "Error binding value in insert " << rc <<" : "<<sqlite3_errmsg(m_pSQLDB);
						
						sqlite3_finalize(insert_stmt);
						throw std::runtime_error ("Error bindinf value in insert " );
					}
					else {
					//	LOGGER_S(debug) << "Successfully bound string for insert: " << str_val.c_str(); 
					}
					
					
				
				i++;
			}

			out_n_sql_error = sqlite3_step(insert_stmt);
			if (SQLITE_DONE != out_n_sql_error) {
				//LOGGER_S(debug) << "insert statement didn't return DONE "<<rc <<" "<< sqlite3_errmsg(m_pSQLDB);
				if (out_n_sql_error == CONSTRAINT)
				{
					LOGGER_S(debug) << "sql duplicate";
					sqlite3_finalize(insert_stmt);
					throw std::runtime_error("ACCOUNT_DUPLICATE");
				}		
				sqlite3_finalize(insert_stmt);
				throw std::runtime_error ("insert statement didn't return DONE ");
			
			}
			else {
				LOGGER_S(debug) << "INSERT complete " ;
			}
			sqlite3_finalize(insert_stmt);
	/*
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
	*/
            return true;
        }

	int AccountDB::SetUpdateCommand(TableName e_table_name, const std::string& str_update_key_field,
			const std::string& str_update_key_value, const json& json_obj, char*& out_cmd)
		{
			std::string szSQLCommand_str;
			szSQLCommand_str.append("update ");
			szSQLCommand_str.append(TABLE_NAME[e_table_name]);
			szSQLCommand_str.append(" set ");

			//LOGGER_S(debug) << "str_update_key_field " << str_update_key_field;
			//LOGGER_S(debug) << "str_update_key_value " << str_update_key_value;
			std::string json_str = json_obj.dump();
			Json::Reader reader;
			Json::Value info;
			reader.parse(json_str, info);

			for (auto const&id : info.getMemberNames())
			{
				szSQLCommand_str.append(id);
				szSQLCommand_str.append("= ?,");
			}
			szSQLCommand_str = szSQLCommand_str.replace(szSQLCommand_str.end() - 1, szSQLCommand_str.end(), " where ");
			szSQLCommand_str.append(str_update_key_field);
			szSQLCommand_str.append(" = ?");
			LOGGER_S(debug) << "szSQLCommand_str " << szSQLCommand_str;
			strcpy(out_cmd, szSQLCommand_str.c_str());

			return info.size();
		}

        bool AccountDB::Update(TableName e_table_name, const std::string& str_update_key_field,
                 const std::string& str_update_key_value, const json& json_obj)
        {
            char szSQLCommand[SQL_CMD_LEN];
            char *pSQLCommand = &szSQLCommand[0];
			
			int count = SetUpdateCommand(e_table_name, str_update_key_field, str_update_key_value, json_obj, pSQLCommand);  //indo.size()
			// For the insert and select, we will prepare statements
			sqlite3_stmt *update_stmt = NULL;
			// SQLite return value
			int rc;
			
			rc = sqlite3_prepare_v2(m_pSQLDB, szSQLCommand, -1, &update_stmt, NULL);
			if (SQLITE_OK != rc) {
				//LOGGER_S(debug) << "Can't prepare insert statment " << szSQLCommand << " " << rc << " : " << sqlite3_errmsg(m_pSQLDB);
			 	throw std::runtime_error("Can't prepare insert statement");
			
			}
		
			int i = 1;  // For bind_text index 
			for (auto it = json_obj.begin(); it != json_obj.end(); ++it) {

				std::string str_val = it.value().dump();
				std::string str_key = it.key();
							
				//The NULL is "Don't attempt to free() the value when it's bound", since it's on the stack here
				
					rc = sqlite3_bind_text(update_stmt, i, str_val.c_str(), str_val.size(), SQLITE_TRANSIENT);
					if (SQLITE_OK != rc) {
						
						//LOGGER_S(debug) << "Error binding value in insert: " << rc << " : " << sqlite3_errmsg(m_pSQLDB);		
						sqlite3_finalize(update_stmt);
						throw std::runtime_error("Error binding value in update ");
						
					}
					else {
						//LOGGER_S(debug) << "Successfully bound string for insert: " << str_val.c_str();	
					}
			
				i++;
			}
			
			//value_str = SetSQLBindText(str_update_key_value);
			std::string value_str;
			value_str = "\"";
			value_str.append(str_update_key_value);
			value_str.append("\"");
			rc = sqlite3_bind_text(update_stmt, i, value_str.c_str(), value_str.size(), SQLITE_TRANSIENT);
			if (SQLITE_OK != rc) {

                                //LOGGER_S(debug) << "Error binding value in insert: " << rc << " : " << sqlite3_errmsg(m_pSQLDB);
				sqlite3_finalize(update_stmt);
                         	throw std::runtime_error("Error binding value in update ");               
                        }
                         else {
                               	LOGGER_S(debug) << "Successfully bound string for update: " << value_str;
                        }

			
			rc = sqlite3_step(update_stmt);
			if (SQLITE_DONE != rc) {
				//LOGGER_S(debug) << "update statement didn't return DONE " << rc << " : " << sqlite3_errmsg(m_pSQLDB);
				sqlite3_finalize(update_stmt);
				throw std::runtime_error("update statemet didn't return DONE ");
			
			}
			else {
				LOGGER_S(debug) << "UPDATE completed";
			}

			sqlite3_finalize(update_stmt);
	/*
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
         */  
            return true;
        }
	
	void AccountDB::SetDeleteCommand(TableName e_table_name, const std::string& str_delete_key_field,
                        const std::string& str_delete_key_value,  char*& out_cmd)
        {
                        std::string szSQLCommand_str;
                        szSQLCommand_str.append("delete from ");
                        szSQLCommand_str.append(TABLE_NAME[e_table_name]);
                        szSQLCommand_str.append(" where ");
                        
			szSQLCommand_str.append(str_delete_key_field);
                        szSQLCommand_str.append(" = ? ");
                        //LOGGER_S(debug) <<  "szSQLCommand_str " << szSQLCommand_str;
                        strcpy(out_cmd, szSQLCommand_str.c_str());


        }


	void AccountDB::sqliteCallbackFunc(void* ,const char* statement) {
    		
		//LOGGER_S(debug)<<"sqliteCallback " << statement;
	}
	
void AccountDB::findAndReplaceAll( std::string & data, const std::string& toSearch, const  std::string& replaceStr)
{
    // Get the first occurrence
    int pos = data.find(toSearch);
    // Repeat till end is reached
    while( pos != std::string::npos)
    {
        // Replace this occurrence of Sub String
        data.replace(pos, toSearch.size(), replaceStr);
        // Get the next occurrence from the current position
        pos =data.find(toSearch, pos + replaceStr.size());
    }
}

        bool AccountDB::Delete(TableName e_table_name, const std::string& str_delete_key_field,
                 const std::string& str_delete_key_value)
        {   
	    char szSQLCommand[SQL_CMD_LEN];            
	    char *pSQLCommand = &szSQLCommand[0];
           
            SetDeleteCommand(e_table_name, str_delete_key_field, str_delete_key_value, pSQLCommand);

            // For the insert and select, we will prepare statements
            sqlite3_stmt *delete_stmt = NULL;
            // SQLite return value
            int rc;
            rc = sqlite3_prepare_v2(m_pSQLDB, szSQLCommand, -1, &delete_stmt, NULL);
	    if (SQLITE_OK != rc) {
		LOGGER_S(debug) << "Can't prepare delete statment " << szSQLCommand << " " << rc << " : " << sqlite3_errmsg(m_pSQLDB);
		throw std::runtime_error("Can't prepare delete statement");
	    }   
          
	std::string data = str_delete_key_value; 
	findAndReplaceAll(data, "\\", "\\\\");
	findAndReplaceAll(data, "\"", "\\\"");
 	//LOGGER_S(debug) << "data " << data; 
            std::string test = "\"";
	    test.append(data);
	    test.append("\"");

            rc = sqlite3_bind_text(delete_stmt, 1, test.c_str(), test.size(), SQLITE_TRANSIENT);
            if (SQLITE_OK != rc) {

                LOGGER_S(debug) << "Error binding value in delete: " << rc << " : " << sqlite3_errmsg(m_pSQLDB);
                sqlite3_finalize(delete_stmt);
                throw std::runtime_error("Error binding value in delete ");

            }
            else {
                //LOGGER_S(debug) << "Successfully bound string for delete: " << str_delete_key_value.c_str();
            }
            rc = sqlite3_step(delete_stmt);
	    //std::string stmt_str = sqlite3_sql(delete_stmt);
            if (SQLITE_DONE != rc) {
                LOGGER_S(debug) << "delete statement didn't return DONE " << rc << " : " << sqlite3_errmsg(m_pSQLDB);
                sqlite3_finalize(delete_stmt);
                throw std::runtime_error("delete statemet didn't return DONE ");

            }
            else {
                LOGGER_S(debug) << "DELETE completed";
		//std::string stmt_str = sqlite3_sql(delete_stmt);
		//LOGGER_S(debug) << "stmt_str " << stmt_str;
		
            }
	    //sqlite3_trace(m_pSQLDB, sqliteCallbackFunc, NULL);
            sqlite3_finalize(delete_stmt);

            

	    /*
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
            */
            return true;
        }

        bool AccountDB::OpenDB()
        { 
            if (sqlite3_open_v2(m_strDBFilePath.c_str(), &m_pSQLDB, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL) != SQLITE_OK){
                
                LOGGER_S(error) << "Open Database Error: " << m_strDBFilePath.c_str() << ":" << sqlite3_errmsg(m_pSQLDB); 
                return false;
            }

            for(const auto &v_table : TABLE_NAME)
            {
                CreateTable(v_table);
                AdjustTable(v_table);
            }

            LOGGER_S(info) << "Database Open Successfully \n";
	   sqlite3_trace(m_pSQLDB, sqliteCallbackFunc, NULL);

            return true;
        }
            
        void AccountDB::CloseDB()
        {
            if(m_pSQLDB)
            {
              sqlite3_close(m_pSQLDB);
              m_pSQLDB = NULL;
            }
        }
        void AccountDB::AdjustTable(const std::string& str_table_name)
        {
            if(str_table_name == "users")            
            {
               //check the column exists
               char* pErrMsg = NULL;
               std::map<std::string, bool> cols;
               LOGGER_S(info) << "PRAGMA table_info(users)";
               if(SQLITE_OK!= sqlite3_exec(m_pSQLDB, "PRAGMA table_info(users)",
                 [](void *veryUsed, int argc, char **argv, char **azColName){
                   std::map<std::string, bool>& cols(*(std::map<std::string, bool>*)veryUsed);
                   cols[argv[1]]= true;
                   return 0;
                 }, &cols, &pErrMsg))
                {
                  LOGGER_S(fatal) << pErrMsg;
                  sqlite3_free(pErrMsg);
                  pErrMsg = NULL;
                }
               if(cols.find("name") == cols.end())
               {
                 LOGGER_S(info) << "TABLE users doesn't contain name column, ALTER TABLE";
                 //alter table
                 std::string cmd = "ALTER TABLE users ADD COLUMN name char(128) default '\"\"'";
                 if(SQLITE_OK != sqlite3_exec(m_pSQLDB, cmd.c_str(), 0, 0, &pErrMsg))
                 {
                   LOGGER_S(fatal) << pErrMsg;
                   sqlite3_free(pErrMsg);
                   pErrMsg = NULL;
                 }
               }
               else
               {
                 LOGGER_S(info) << "TABLE users contains name column, skip ALTER TABLE";
               }
            }
        }
        void AccountDB::CreateTable(const std::string& str_table_name)
        {
            char szSQLCommand[SQL_CMD_LEN];
            char *pSQLCommand = &szSQLCommand[0];

            pSQLCommand += sprintf(pSQLCommand, "create table if not exists %s", str_table_name.c_str());

            if(str_table_name == "users"){
                
               std::string hash_pass = hash::TextEncodeHash("admin");
               
               pSQLCommand += sprintf(pSQLCommand, "(account char(32) not null primary key,");
               pSQLCommand += sprintf(pSQLCommand, "encryptedPassword char(128) null,");
               pSQLCommand += sprintf(pSQLCommand, "accessRightName char(16) null,");
               pSQLCommand += sprintf(pSQLCommand, "name char(128) default '\"\"',");
               //pSQLCommand += sprintf(pSQLCommand, "photoLink char(128) default '\"\"',");
               pSQLCommand += sprintf(pSQLCommand, "description text '""');");
               pSQLCommand += sprintf(pSQLCommand, "insert or ignore into users ");
               pSQLCommand += sprintf(pSQLCommand, "(account,encryptedPassword,accessRightName,description) ");
               pSQLCommand += sprintf(pSQLCommand, "values('\"admin\"','\"%s\"','\"admin\"','\"\"');", hash_pass.c_str());
            }
            else if(str_table_name == "accessRights"){

               json::object_t DEFAULT_PERMISSIONS = Config::getInstance().DEFAULT_PERMISSIONS;
               json j_features(DEFAULT_PERMISSIONS);
               std::string permissions = j_features.dump();

	       json::object_t DEFAULT_LDAP_PERMISSIONS = LdapConfig::getInstance().DEFAULT_PERMISSIONS;
	       json j_features_ldap(DEFAULT_LDAP_PERMISSIONS);
	       std::string permissions_ldap = j_features_ldap.dump();
                
               pSQLCommand += sprintf(pSQLCommand, "(accessRightName char(32) not null primary key,");
               pSQLCommand += sprintf(pSQLCommand, "permissions char(32) null,");
               pSQLCommand += sprintf(pSQLCommand, "description text '""');");
               pSQLCommand += sprintf(pSQLCommand, "insert or ignore into accessRights(accessRightName,permissions,description) ");
               pSQLCommand += sprintf(pSQLCommand, "values('\"admin\"','%s','\"\"');", permissions.c_str());
	       pSQLCommand += sprintf(pSQLCommand, "insert or ignore into accessRights(accessRightName,permissions,description) ");
	       pSQLCommand += sprintf(pSQLCommand, "values('\"AD_user\"','%s','\"\"');", permissions_ldap.c_str());
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


