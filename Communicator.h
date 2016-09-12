#pragma once

// std
#include <atomic>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <vector>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <iomanip>

// boost
#include <boost/network/include/http/server.hpp>

// self
#include "AccountManager.h"


class Settings;
struct RequestHandler;
typedef boost::network::http::async_server<RequestHandler> Server;

class Communicator
{
public:
    Communicator(const Settings& settings, gorilla::account::AccountManager& accountManager);
    ~Communicator();

    void ProcessRequest(const Server::request& request,
        Server::connection_ptr& connection_ptr);

private:
    const Settings& settings_;

    gorilla::account::AccountManager& m_accountManager;

    std::map<std::string,  std::function<Server::connection::status_t(
        const Server::request&, std::string&, std::string&)> > request_handler_mapping_;
  
    std::atomic<bool> is_body_ready_;
    bool is_reply_ready_;
    std::mutex request_mtx_;
    std::mutex reply_mtx_;
    std::condition_variable request_cv_;
    std::condition_variable reply_cv_;

    std::string m_str_account;
    std::string m_str_password;

    gorilla::account::ErrorReply m_error_reply;
    
private:

    void RequestHandlers();

    void ReadChunk(Server::connection_ptr& connection_ptr, size_t left2read,
        std::string& body_str);

    void ReadSingleBody(Server::connection::input_range& input,
        boost::system::error_code& ec, std::size_t& bytes_transferred,
        Server::connection_ptr& connection_ptr,
        size_t left2read, std::string& body_str);

    bool CheckURIPath(const std::string &str_check_tag, 
        std::string &out_str_uri_path);

    std::string SetURIPath(const std::string &str_request_method, 
        const std::string &str_uri_path);

    void ParseURIFields(const std::string &str_uri_path, std::list<std::string>& out_lst_fields);

    std::string GetName(const std::string &str_check_tag, 
        const std::string &str_uri_path) const;

    void SendHeader(Server::connection_ptr& connection_ptr, Server::connection::status_t status_code);

    void AsyncWriteCallback(const boost::system::error_code& errorCode);

    bool GetIsAsyncWriteReady();

    void SendReply(Server::connection_ptr& connection_ptr, std::string &reply_str);  

    void GetAuthorization(const Server::request& request, 
        std::string& out_str_account, std::string& out_str_password);

    Server::connection::status_t GetUsers(const Server::request& request, 
        std::string &request_str, std::string &reply_str);

    Server::connection::status_t AddUser(const Server::request& request, 
        std::string &request_str, std::string &reply_str);

    Server::connection::status_t GetUser(const Server::request& request, 
        std::string &request_str, std::string &reply_str);

    Server::connection::status_t UpdateUser(const Server::request& request, 
        std::string &request_str, std::string &reply_str);

    Server::connection::status_t DeleteUser(const Server::request& request, 
        std::string &request_str, std::string &reply_str);

    Server::connection::status_t GetUserFeatures(const Server::request& request, 
        std::string &request_str, std::string &reply_str);

    Server::connection::status_t GetLevels(const Server::request& request, 
        std::string &request_str, std::string &reply_str);

    Server::connection::status_t AddLevel(const Server::request& request, 
        std::string &request_str, std::string &reply_str);

    Server::connection::status_t GetLevel(const Server::request& request, 
        std::string &request_str, std::string &reply_str);

    Server::connection::status_t UpdateLevel(const Server::request& request, 
        std::string &request_str, std::string &reply_str);

    Server::connection::status_t DeleteLevel(const Server::request& request, 
        std::string &request_str, std::string &reply_str);

    
};

