#include <stdio.h>
#include <atomic>
#include <chrono>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>


// Required by cpp-netlib v1.1.2
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

// Required by cpp-netlib v1.2.0
#include <boost/network/include/http/server.hpp>
#include <boost/network/uri.hpp>


#ifdef __unix__
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#include <sys/types.h>
#include <unistd.h>
#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __unix__

//common
#include "gorilla/log/logger.h"
#include "gorilla/log/logger_config.h"
#include "settings.h"
#include "Communicator.h"


using namespace gorilla::log;
static const std::string kDefaultSettingPath("./settings.ini");
std::atomic<bool> g_running(true);// This is needed to terminate the worker queue, and must be visible to the signal handler.


/**
 * request + connection encapsulation (work item)
 */
struct RequestData {
    const Server::request request_;
    Server::connection_ptr connection_ptr_;

    typedef std::shared_ptr<RequestData> pointer;

    RequestData(Server::request req, Server::connection_ptr conn)
        : request_(std::move(req))
        , connection_ptr_(std::move(conn))
    {
    }
};


/**
 * A basic work queue
 */
struct WorkQueue {
    std::list<RequestData::pointer> list_;
    std::mutex mutex_;

    std::condition_variable request_cv_;
    inline void put(const RequestData::pointer& request_data_ptr)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        list_.push_back(request_data_ptr);
        request_cv_.notify_one();
        (void)lock;
    }

    inline RequestData::pointer get()
    {
        std::unique_lock<std::mutex> lock(mutex_);

        RequestData::pointer request_data_ptr;
        if (!list_.empty()) {
            request_data_ptr = list_.front();
            list_.pop_front();
        }
        else
            request_cv_.wait(lock);

        (void)lock;

        return request_data_ptr;
    }
};


struct RequestHandler {
    
    WorkQueue& queue_;

    RequestHandler(WorkQueue& queue)
        : queue_(queue)
    {
    }

    /**
     * Feed the work queue
     *
     * @param req
     * @param conn
     */
    void operator()(Server::request const& req,
        const Server::connection_ptr& conn)
    {
        queue_.put(std::make_shared<RequestData>(req, conn));
    }
};


/**
 * Clean shutdown signal handler
 *
 * @param error
 * @param signal
 * @param server_data_ptr
 */
void ShutMeDown(const boost::system::error_code& error, int signal,
                  boost::shared_ptr<Server> server_data_ptr)
{
    if (!error)
        server_data_ptr->stop();
}

/**
 * Process request; worker (thread)
 *
 * @param queue
 */
void ProcessRequest(WorkQueue& queue, const Settings &settings, gorilla::account::AccountManager& accountManager)
{
    while (g_running) {
        
        RequestData::pointer request_ptr(queue.get());
            
        Communicator iotCommunicator(settings,accountManager);
        
        if (request_ptr) {
             
            iotCommunicator.ProcessRequest(
                request_ptr->request_, request_ptr->connection_ptr_);
        } 
    }
}

std::string PasswordCallback(std::size_t max_length,
   boost::asio::ssl::context_base::password_purpose purpose)
{
    return std::string("test");
}

int main(int argc, char* argv[])
{

    try{

        const Settings settings(kDefaultSettingPath);

        /* initialize logger */       
        logger_config config;      
        
        logger_keeper *pKeeper = get_keeper();
        pKeeper->add_file_logger();
        pKeeper->add_console_logger();
        config.configure_with(boost::filesystem::path(kDefaultSettingPath));
        
        LOGGER_S(info)<< "Account Server Start ... ";
    
        /* intial */
        auto thread_group_ptr( boost::make_shared<boost::thread_group>());
        auto io_service_ptr(boost::make_shared<boost::asio::io_service>());
        auto io_service_work_ptr(

        boost::make_shared<boost::asio::io_service::work>(
                 boost::ref(*io_service_ptr)));
        
         /* create io server thread */
         {
                int n_threads = 5;
                while (0 < n_threads--) {
                thread_group_ptr->create_thread(
                     boost::bind(&boost::asio::io_service::run, io_service_ptr));
                }
         }
   
         // create request queue & communicator thread
         WorkQueue queue;
         gorilla::account::AccountManager accountManager;
         {
                int n_threads = 5;
                while (0 < n_threads--) {

                     thread_group_ptr->create_thread(boost::bind(ProcessRequest,
                       boost::ref(queue),boost::ref(settings),boost::ref(accountManager)));      
                }
         }


        /* setup the async Server (can get one request and send several response) */
        RequestHandler handler(queue);
        boost::shared_ptr<Server> server_data_ptr;

       
        /* set https ssl */
        if (settings.enable_https_ == 1) {

                boost::shared_ptr<boost::asio::ssl::context> ctx =
                    boost::make_shared<boost::asio::ssl::context>(
                        boost::asio::ssl::context::sslv23);
                ctx->set_options(boost::asio::ssl::context::default_workarounds |
                    boost::asio::ssl::context::no_sslv2 |
                    boost::asio::ssl::context::single_dh_use);

                /* Set keys */
                ctx->set_password_callback(PasswordCallback);
                ctx->use_certificate_chain_file(settings.private_key_path_);
                // ctx->use_private_key_file(settings.private_key_path_,
                //     asio::ssl::context::pem);
                ctx->use_private_key_file("server.pem",
                    boost::asio::ssl::context::pem);
                ctx->use_tmp_dh_file(settings.dh_file_path_);

                server_data_ptr = boost::make_shared<Server>(
                    Server::options(handler)
                    .address(settings.server_ip_)
                    .port(std::to_string(settings.server_port_))
                    .io_service(io_service_ptr)
                    .reuse_address(true)
                    .thread_pool(
                        boost::make_shared<boost::network::utils::thread_pool>(
                            2, io_service_ptr, thread_group_ptr))
                    .context(ctx));
        } 
        else {

                server_data_ptr = boost::make_shared<Server>(
                    Server::options(handler)
                    .address(settings.server_ip_)
                    .port(std::to_string(settings.server_port_))
                    .io_service(io_service_ptr)
                    .reuse_address(true)
                    .thread_pool(
                        boost::make_shared<boost::network::utils::thread_pool>(
                        2, io_service_ptr, thread_group_ptr)));
        }


        boost::asio::signal_set signals(*io_service_ptr, SIGINT, SIGTERM);
        signals.async_wait(boost::bind(ShutMeDown, _1, _2, server_data_ptr));

        /* run the async Server */
        server_data_ptr->run();

        /* we are stopped - shutting down */
        thread_group_ptr->interrupt_all();

        io_service_work_ptr.reset();
        io_service_ptr->stop();

        thread_group_ptr->join_all();

        exit(EXIT_SUCCESS);

    }
    catch(const std::exception& e){

        //LOGGER() << "Abnormal termination - exception:" << e.what();
        exit(EXIT_FAILURE);
    }

    LOGGER()<< "IOT Server Finish ... \n";
}
