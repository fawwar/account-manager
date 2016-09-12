#include <sstream>
#include <chrono>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/uuid/sha1.hpp>

#include "hash_codec.hpp"
#include "gorilla/log/logger.h"
#include "gorilla/log/logger_config.h"
#include "settings.h"


#include "Communicator.h"

using namespace gorilla::log;
namespace
{
	const int CONNECTION_READ_TIMEOUT_MS = 10000;
    const int CONNECTION_WRITE_TIMEOUT_MS = 10000;
}


Communicator::Communicator(const Settings& settings, gorilla::account::AccountManager& accountManager)
    : settings_(settings)
    , m_accountManager(accountManager)
    , is_body_ready_(false)
    , is_reply_ready_(false)
{
    RequestHandlers();
}

Communicator::~Communicator()
{
}

void Communicator::ProcessRequest(const Server::request& request,
    Server::connection_ptr& connection_ptr)
{
    // print http request message
    std::ostringstream oss;
    std::string account, password;
    
    LOGGER_S(info) << "From: " << "\n" <<request.source << " \n"
       << request.method << " \n" << request.destination << "\n"
       << request.http_version_major << "\n" << request.http_version_minor;

    
    GetAuthorization(request, m_str_account, m_str_password);

    
    int cl;
    Server::request::headers_container_type const &hs = request.headers;
    for(auto it = hs.begin(); it != hs.end(); ++it) {
        if(boost::to_lower_copy(it->name)=="content-length") {
            cl = boost::lexical_cast<int>(it->value);
            break;
        }
    }
    
    std::string body_str;

    if (request.method == "POST" || request.method == "PUT"){

        LOGGER_S(debug) << "Body length: " << cl << " byte";
        is_body_ready_ = false;
		
		/* make sure read wait first then to do read notify_one*/
        std::unique_lock<std::mutex> lck(request_mtx_);
        ReadChunk(connection_ptr, cl, boost::ref(body_str));

        if(! is_body_ready_) {            
            LOGGER_S(debug) << "Wait For Request Body...";  
            
			/* read wait can unlock request_mtx_ */
			/* use wait_for can prevent connect time out, 
			cause process block here (because read chuck no data to read, then can't notify_one)*/
			request_cv_.wait_for(lck, std::chrono::milliseconds(CONNECTION_READ_TIMEOUT_MS));        
        }
   
        /* web post content */	
        LOGGER_S(debug)<< "Web Post Content> " <<"\n"<< body_str;
    }


    std::string reply_str;
    Server::connection::status_t status_code =
            Server::connection::internal_server_error;
    
    if(request.method != "OPTIONS"){

        const boost::network::uri::uri uri_instance(
            std::string("http://127.0.0.1" + request.destination));

        LOGGER_S(info) << "URI Path> " << uri_instance.path() << "\n";

        // search iot function
        std::string new_uri_instance_path = SetURIPath(request.method, uri_instance.path());

        auto it = request_handler_mapping_.find(new_uri_instance_path);
        if (it != request_handler_mapping_.end()) {
            status_code = it->second(request, body_str, reply_str);
        } 
        else {
            LOGGER_S(info) << "Command not found";
            status_code = Server::connection::bad_request;
        }
    }
    else{

        LOGGER_S(info) << "IS OPTIONS \n";
        status_code = Server::connection::ok;
    }
    

    // http reply
    SendHeader(connection_ptr, status_code);
    SendReply(connection_ptr, reply_str);
    
}

void Communicator::RequestHandlers()
{
     request_handler_mapping_ =
     {   
            {
                "GET/users",
                std::bind(&Communicator::GetUsers, this,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)    
            },
            {
                "POST/users",
                std::bind(&Communicator::AddUser, this,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)    
            },
            {
                "GET/users/name",
                std::bind(&Communicator::GetUser, this,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)    
            },
            {
                "PUT/users/name",
                std::bind(&Communicator::UpdateUser, this,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)    
            },
            {
                "DELETE/users/name",
                std::bind(&Communicator::DeleteUser, this,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)    
            },
            {
                "GET/users/name/features",
                std::bind(&Communicator::GetUserFeatures, this,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)    
            },
            {
                "GET/levels",
                std::bind(&Communicator::GetLevels, this,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)    
            },
            {
                "POST/levels",
                std::bind(&Communicator::AddLevel, this,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)    
            },
            {
                "GET/levels/name",
                std::bind(&Communicator::GetLevel, this,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)    
            },
            {
                "PUT/levels/name",
                std::bind(&Communicator::UpdateLevel, this,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)    
            },
            {
                "DELETE/levels/name",
                std::bind(&Communicator::DeleteLevel, this,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)    
            } 
      };
}

void Communicator::ReadChunk(Server::connection_ptr& connection_ptr,
    size_t left2read, std::string& body_str)
{
    /* call back function */
    connection_ptr->read(boost::bind(&Communicator::ReadSingleBody, this,
        _1, _2, _3, connection_ptr, left2read, boost::ref(body_str)));
}

void Communicator::ReadSingleBody(Server::connection::input_range& input,
    boost::system::error_code& ec, std::size_t& bytes_transferred,
    Server::connection_ptr& connection_ptr, size_t left2read,
    std::string& body_str)
{
    if (!ec) {
        
        body_str.append(boost::begin(input), bytes_transferred);
        size_t left = left2read - bytes_transferred;

        /* read remain data */
        if (left > 0) {
            ReadChunk(connection_ptr, left, boost::ref(body_str));
        } 
        else {
            
			/*make sure afeter read notify_one */
			std::unique_lock<std::mutex> lck(request_mtx_);
            is_body_ready_ = true;
			request_cv_.notify_one();
        }
    }
}

bool Communicator::CheckURIPath(const std::string &str_check_tag, 
    std::string &out_str_uri_path)
{
    if(out_str_uri_path.length() <= str_check_tag.length())
        return false;

 
    std::size_t pos = out_str_uri_path.find(str_check_tag);
    if (pos != std::string::npos){

        /* ex: Get/users/ */
        std::string first_half = out_str_uri_path.substr (0, pos + str_check_tag.length());
        //out_str_uri_path.append("name");

        /* ex: name123/features */
        std::string second_half = out_str_uri_path.substr (first_half.length(), out_str_uri_path.length());

        out_str_uri_path.clear();
        pos = second_half.find("/");
        if (pos != std::string::npos){
            second_half = second_half.substr (pos , second_half.length());
            out_str_uri_path = first_half + "name" +  second_half;
        }
        else{
            out_str_uri_path = first_half + "name";
        }

        return true;
    }

    return false;
}

std::string Communicator::SetURIPath(const std::string &str_request_method, 
        const std::string &str_uri_path)
{
    std::string new_uri_path;

    new_uri_path.append(str_request_method);
    new_uri_path.append(str_uri_path);
    
    
    CheckURIPath("/users/", new_uri_path);
    CheckURIPath("/levels/", new_uri_path);

    LOGGER_S(debug) << "SetURIPath = " << new_uri_path;
   
    return new_uri_path;
}

void Communicator::ParseURIFields(const std::string &str_uri_path, std::list<std::string>& out_lst_fields)
{
    const std::string FIELD_FORMATE[] = {"fields%5B%5D=", "fields="};    

    std::string tmp_uri_path;
    
    /* get exclude filed fomate word content */
    for(auto fieldword : FIELD_FORMATE){

        std::size_t pos = str_uri_path.find(fieldword);
        if (pos != std::string::npos){
            tmp_uri_path = str_uri_path.substr(fieldword.length());
            break;
        }
    }

    while(!tmp_uri_path.empty()){

        std::string field;
		std::size_t pos = tmp_uri_path.find(",");
		if (pos != std::string::npos){
			 field = tmp_uri_path.substr(0, pos);
			 tmp_uri_path = tmp_uri_path.substr(pos + 1);
		}
		else{
			field = tmp_uri_path;
			tmp_uri_path = tmp_uri_path.substr(tmp_uri_path.length());
		}

        out_lst_fields.push_back(field);
    }

    for(auto& it : out_lst_fields){
        LOGGER_S(debug) << "field = " << it;
    }

    
}

std::string Communicator::GetName(const std::string &str_check_tag, 
    const std::string &str_uri_path) const
{
    std::string name;
    std::size_t pos = str_uri_path.find(str_check_tag);

    if (pos != std::string::npos){ 

        name = str_uri_path.substr(pos + str_check_tag.length());

        pos = name.find("/");
        if (pos != std::string::npos)
            name = name.substr(0, pos);
    }
    
    return name;
}

void Communicator::SendHeader(Server::connection_ptr& connection_ptr, Server::connection::status_t status_code)
{
    static Server::response_header headers[] = 
    {
        {"Content-Type","application/json; charset=utf-8"},
        {"Access-Control-Allow-Origin","*"},
        {"Access-Control-Allow-Methods","GET, POST, PUT, DELETE, OPTIONS"},
        {"Access-Control-Allow-Headers","Content-Type, AUTHORIZATION"},
    };

    const int HEADERSIZE = (sizeof(headers)/sizeof(*headers));


    LOGGER_S(debug) << "SendHeader \n";
    
    connection_ptr->set_status(status_code);
    connection_ptr->set_headers(boost::make_iterator_range(headers, headers + HEADERSIZE));
}

void Communicator::AsyncWriteCallback(const boost::system::error_code& errorCode)
{
     LOGGER_S(debug) << "AsyncWriteCallback err_code:" << errorCode.value();        
     {        
        std::lock_guard<std::mutex> lck(reply_mtx_); 
        is_reply_ready_ = true;
     }    

     reply_cv_.notify_one();
}

bool Communicator::GetIsAsyncWriteReady() 
{ 
    return is_reply_ready_; 
}

void Communicator::GetAuthorization(const Server::request& request, 
        std::string& out_str_account, std::string& out_str_password)
{
    out_str_account = "";
    out_str_password = "";
    
    for (const auto& header : request.headers) {

            if(header.name == "Authorization"){

                const std::string FIELD = "Basic ";
                std::string authority = header.value;
                std::size_t found_pos = authority.find(FIELD);
                if (found_pos != std::string::npos){
                                        
                    /* paser authority */
                    authority = authority.substr(found_pos + FIELD.size());

                    /* base64 decode */
                    std::string decode_authority = gorilla::hash::DecodeBase64(authority);
    
                    /* parser account & password */
                    found_pos = decode_authority.find(":");
                    if (found_pos != std::string::npos){

                        out_str_account = decode_authority.substr(0, found_pos);
                        out_str_password = decode_authority.substr(found_pos + 1);

                        LOGGER_S(info) << "Account:Pass > " << out_str_account <<":" << out_str_password;    
                    }  
                }
                
                break;
            }         
    }
}

void Communicator::SendReply(Server::connection_ptr& connection_ptr, std::string &reply_str)
{
    if (reply_str.size() == 0)
        reply_str = std::string("\n");

    LOGGER_S(debug) << "Server Reply Content> " << reply_str << "\n";   

    std::unique_lock<std::mutex> lck(reply_mtx_);
    is_reply_ready_ = false;
    connection_ptr->write(reply_str, boost::bind(&Communicator::AsyncWriteCallback, this, _1));
    
    reply_cv_.wait_for(lck, std::chrono::milliseconds(CONNECTION_WRITE_TIMEOUT_MS), 
        boost::bind(&Communicator::GetIsAsyncWriteReady, this));

    
    LOGGER_S(debug) << "Server Reply End> ";
}

Server::connection::status_t Communicator::GetUsers(const Server::request& request, 
        std::string &request_str, std::string &reply_str)
{
    gorilla::account::Error err;
    
    const boost::network::uri::uri uri_instance(
        std::string("http://127.0.0.1" + request.destination));

    LOGGER_S(debug) << "GetUsers = " << m_str_account << "," << m_str_password;

    std::string level;
    m_accountManager.GetUserLevel(m_str_account, level);

    LOGGER_S(debug) << "GetUsers Level = " << level; 
    
    if(level == "admin"){
        
        bool res = m_accountManager.VerifyAccount(m_str_account, m_str_password);
        if(res){
            err = m_accountManager.GetUsers(reply_str);
        }
        else{
           err = gorilla::account::UNAUTHORIZED;
           reply_str = m_error_reply.GetError("Account Or Password Error", "<Communicator::GetUsers> UNAUTHORIZED");
        }
    }
    else{
        err = gorilla::account::FORBIDDEN;
        reply_str = m_error_reply.GetError("Need To Administrator Level Permission", "<Communicator::GetUsers> FORBIDDEN");
    }

    return (Server::connection::status_t)err;
}

Server::connection::status_t Communicator::AddUser(const Server::request& request, 
        std::string &request_str, std::string &reply_str)
{
    gorilla::account::Error err;
    
    const boost::network::uri::uri uri_instance(
        std::string("http://127.0.0.1" + request.destination));

    LOGGER_S(debug) << "AddUsers = " << m_str_account << "," << m_str_password;  

    std::string level;
    m_accountManager.GetUserLevel(m_str_account, level);
    
    if(level == "admin"){
        
        bool res = m_accountManager.VerifyAccount(m_str_account, m_str_password);
        if(res){
            err = m_accountManager.AddUser(request_str, reply_str);
        }
        else{
           err = gorilla::account::UNAUTHORIZED; 
           reply_str = m_error_reply.GetError("Account Or Password Error", "<Communicator::AddUser> UNAUTHORIZED");
        }
    }
    else{
        
        err = gorilla::account::FORBIDDEN;
        reply_str = m_error_reply.GetError("Need To Administrator Level Permission", "<Communicator::AddUser> FORBIDDEN");
    }

    return (Server::connection::status_t)err;
}

Server::connection::status_t Communicator::GetUser(const Server::request& request, 
        std::string &request_str, std::string &reply_str)
{
    gorilla::account::Error err;
    
    const boost::network::uri::uri uri_instance(
        std::string("http://127.0.0.1" + request.destination));

    LOGGER_S(debug) << "GetUser = " << m_str_account << "," << m_str_password;  

    bool res = m_accountManager.VerifyAccount(m_str_account, m_str_password);
    if(res){

        std::string level;
        m_accountManager.GetUserLevel(m_str_account, level);
        std::string user_name = GetName("/users/", uri_instance.path());  
        
        if(m_str_account == user_name || level == "admin")
            err = m_accountManager.GetUser(user_name, reply_str);
        else{

            err = gorilla::account::FORBIDDEN;

            std::ostringstream oss;
            oss << "Need To " << user_name <<" Or Adminstrator Level Permissions";
            reply_str = m_error_reply.GetError(oss.str(), "<Communicator::GetUser> FORBIDDEN");
        }
    }
    else{
       err = gorilla::account::UNAUTHORIZED; 
       reply_str = m_error_reply.GetError("Account Or Password Error", "<Communicator::GetUser> UNAUTHORIZED");
    }

    return (Server::connection::status_t)err;
}

Server::connection::status_t Communicator::UpdateUser(const Server::request& request, 
        std::string &request_str, std::string &reply_str)
{
    gorilla::account::Error err;
    
    const boost::network::uri::uri uri_instance(
        std::string("http://127.0.0.1" + request.destination));

    LOGGER_S(debug) << "UpdateUser = " << m_str_account << "," << m_str_password; 

    bool res = m_accountManager.VerifyAccount(m_str_account, m_str_password);
    if(res){

        std::string level;
        m_accountManager.GetUserLevel(m_str_account, level);
        std::string user_name = GetName("/users/", uri_instance.path());  
       
        if(m_str_account == user_name || level == "admin"){
            err = m_accountManager.UpdateUser(user_name, level, request_str, reply_str);
        }
        else{

            err = gorilla::account::FORBIDDEN;

            std::ostringstream oss;
            oss << "Need To " << user_name <<" Or Adminstrator Level Permissions";
            reply_str = m_error_reply.GetError(oss.str(), "<Communicator::UpdateUser> FORBIDDEN");
        }
    }
    else{
       err = gorilla::account::UNAUTHORIZED; 
       reply_str = m_error_reply.GetError("Account Or Password Error", "<Communicator::UpdateUser> UNAUTHORIZED");
    }
    
    return (Server::connection::status_t)err;
}

Server::connection::status_t Communicator::DeleteUser(const Server::request& request, 
        std::string &request_str, std::string &reply_str)
{
    gorilla::account::Error err;
    
    const boost::network::uri::uri uri_instance(
        std::string("http://127.0.0.1" + request.destination));

    LOGGER_S(debug) << "DeleteUser = " << m_str_account << "," << m_str_password;  
    bool res = m_accountManager.VerifyAccount(m_str_account, m_str_password);
    if(res){

        std::string level;
        m_accountManager.GetUserLevel(m_str_account, level);
        std::string user_name = GetName("/users/", uri_instance.path());  

        if(level == "admin"){

            /* can't delete self */
            if(m_str_account == user_name){

                err = gorilla::account::FORBIDDEN;
                reply_str = m_error_reply.GetError("Can't Delete Self Account", "<Communicator::DeleteUser> FORBIDDEN");
            }
            else
                err = m_accountManager.DeleteUser(user_name, reply_str);
        }
        else{

            err = gorilla::account::FORBIDDEN;

            std::ostringstream oss;
            oss << "Need To " << user_name <<" Or Adminstrator Level Permissions";
            reply_str = m_error_reply.GetError(oss.str(), "<Communicator::DeleteUser> FORBIDDEN");
        }  
    }
    else{
       err = gorilla::account::UNAUTHORIZED; 
       reply_str = m_error_reply.GetError("Account Or Password Error", "<Communicator::DeleteUser> UNAUTHORIZED");
    }
    
    return (Server::connection::status_t)err;    
}

Server::connection::status_t Communicator::GetUserFeatures(const Server::request& request, 
        std::string &request_str, std::string &reply_str)
{
    gorilla::account::Error err;
    
    const boost::network::uri::uri uri_instance(
        std::string("http://127.0.0.1" + request.destination));

    LOGGER_S(debug) << "GetUserFeatures = " << m_str_account << "," << m_str_password;  

    bool res = m_accountManager.VerifyAccount(m_str_account, m_str_password);
    if(res){

        std::string level;
        m_accountManager.GetUserLevel(m_str_account, level);
        std::string user_name = GetName("/users/", uri_instance.path());  

        std::list<std::string> fields;
        ParseURIFields(uri_instance.query(), fields);

        if(m_str_account == user_name || level == "admin")
            err = m_accountManager.GetUserFeatures(user_name, fields, reply_str);
        else{

            err = gorilla::account::FORBIDDEN;

            std::ostringstream oss;
            oss << "Need To " << user_name <<" Or Adminstrator Level Permissions";
            reply_str = m_error_reply.GetError(oss.str(), "<Communicator::GetUserFeatures> FORBIDDEN");
        }
    }
    else{
       err = gorilla::account::UNAUTHORIZED; 
       reply_str = m_error_reply.GetError("Account Or Password Error", "<Communicator::GetUserFeatures> UNAUTHORIZED");
    }

    return (Server::connection::status_t)err;    
}

Server::connection::status_t Communicator::GetLevels(const Server::request& request, 
        std::string &request_str, std::string &reply_str)
{
    gorilla::account::Error err;
    
    const boost::network::uri::uri uri_instance(
        std::string("http://127.0.0.1" + request.destination));

    LOGGER_S(debug) << "GetLevels = " << m_str_account << "," << m_str_password;

    std::string level;
    m_accountManager.GetUserLevel(m_str_account, level);
    if(level == "admin"){
        
        bool res = m_accountManager.VerifyAccount(m_str_account, m_str_password);
        if(res){
            err = m_accountManager.GetLevels(reply_str);
        }
        else{
           err = gorilla::account::UNAUTHORIZED;
           reply_str = m_error_reply.GetError("Account Or Password Error", "<Communicator::GetLevels> UNAUTHORIZED");
        }
    }
    else{
        err = gorilla::account::FORBIDDEN;
        reply_str = m_error_reply.GetError("Need To Administrator Level Permission", "<Communicator::GetLevels> FORBIDDEN");
    }

    return (Server::connection::status_t)err;    
}

Server::connection::status_t Communicator::AddLevel(const Server::request& request, 
        std::string &request_str, std::string &reply_str)
{
    gorilla::account::Error err;
    
    const boost::network::uri::uri uri_instance(
        std::string("http://127.0.0.1" + request.destination));

    LOGGER_S(debug) << "AddLevel = " << m_str_account << "," << m_str_password; 

    std::string level;
    m_accountManager.GetUserLevel(m_str_account, level);
    
    if(level == "admin"){
        
        bool res = m_accountManager.VerifyAccount(m_str_account, m_str_password);
        if(res){
            err = m_accountManager.AddLevel(request_str, reply_str);
        }
        else{
           err = gorilla::account::UNAUTHORIZED; 
           reply_str = m_error_reply.GetError("Account Or Password Error", "<Communicator::AddLevel> UNAUTHORIZED");
        }
    }
    else{
        
        err = gorilla::account::FORBIDDEN;
        reply_str = m_error_reply.GetError("Need To Administrator Level Permission", "<Communicator::AddLevel> FORBIDDEN");
    }

    
    return (Server::connection::status_t)err; 
}

Server::connection::status_t Communicator::GetLevel(const Server::request& request, 
        std::string &request_str, std::string &reply_str)
{
    gorilla::account::Error err;
    
    const boost::network::uri::uri uri_instance(
        std::string("http://127.0.0.1" + request.destination));

    LOGGER_S(debug) << "GetLevel = " << m_str_account << "," << m_str_password;

    std::string level;
    m_accountManager.GetUserLevel(m_str_account, level);
    
    if(level == "admin"){
        
        bool res = m_accountManager.VerifyAccount(m_str_account, m_str_password);
        if(res){

            std::string level_name = GetName("/levels/", uri_instance.path());
            err = m_accountManager.GetLevel(level_name, reply_str);
        }
        else{
           err = gorilla::account::UNAUTHORIZED; 
           reply_str = m_error_reply.GetError("Account Or Password Error", "<Communicator::GetLevel> UNAUTHORIZED");
        }
    }
    else{
        
        err = gorilla::account::FORBIDDEN;
        reply_str = m_error_reply.GetError("Need To Administrator Level Permission",  "<Communicator::GetLevel> FORBIDDEN");
    }

    return (Server::connection::status_t)err; 
}

Server::connection::status_t Communicator::UpdateLevel(const Server::request& request, 
        std::string &request_str, std::string &reply_str)
{
    gorilla::account::Error err;
    
    const boost::network::uri::uri uri_instance(
        std::string("http://127.0.0.1" + request.destination));

    LOGGER_S(debug) << "UpdateLevel = " << m_str_account << "," << m_str_password;

    std::string level;
    m_accountManager.GetUserLevel(m_str_account, level);
    
    if(level == "admin"){
        
        bool res = m_accountManager.VerifyAccount(m_str_account, m_str_password);
        if(res){

            std::string level_name = GetName("/levels/", uri_instance.path());
            err = m_accountManager.UpdateLevel(level_name, request_str, reply_str);
        }
        else{
           err = gorilla::account::UNAUTHORIZED; 
           reply_str = m_error_reply.GetError("Account Or Password Error", "<Communicator::UpdateLevel> UNAUTHORIZED");
        }
    }
    else{
        
        err = gorilla::account::FORBIDDEN;
        reply_str = m_error_reply.GetError("Need To Administrator Level Permission", "<Communicator::UpdateLevel> UNAUTHORIZED");
    }

    return (Server::connection::status_t)err;     
}

Server::connection::status_t Communicator::DeleteLevel(const Server::request& request, 
        std::string &request_str, std::string &reply_str)
{
    gorilla::account::Error err;
    
    const boost::network::uri::uri uri_instance(
        std::string("http://127.0.0.1" + request.destination));

    LOGGER_S(debug) << "UpdateLevel = " << m_str_account << "," << m_str_password;

    std::string level;
    m_accountManager.GetUserLevel(m_str_account, level);
    
    if(level == "admin"){
        
        bool res = m_accountManager.VerifyAccount(m_str_account, m_str_password);
        if(res){

            std::string level_name = GetName("/levels/", uri_instance.path());
            err = m_accountManager.DeleteLevel(level_name, reply_str);
        }
        else{
           err = gorilla::account::UNAUTHORIZED; 
           reply_str = m_error_reply.GetError("Account Or Password Error", "<Communicator::DeleteLevel> UNAUTHORIZED");
        }
    }
    else{
        
        err = gorilla::account::FORBIDDEN;
        reply_str = m_error_reply.GetError("Need To Administrator Level Permission", "<Communicator::DeleteLevel> FORBIDDEN");
    }

    return (Server::connection::status_t)err;
}
