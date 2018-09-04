#include <stdio.h>
#include <atomic>
#include <iostream>
#include <memory>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include "LegacyHttp.h"
//common
#include "gorilla/log/logger.h"
#include "gorilla/log/logger_config.h"
#include "settings.h"
#include "Communicator.h"
#include "Util.h"
#include "Config.h"
#include <http/HttpServer.h>
#include <http/HttpRouter.h>
#include "AdapterHandler.h"

using namespace gorilla::http;
using namespace gorilla::log;
static const std::string kDefaultSettingPath("./settings.ini");

std::atomic<bool> g_running(true);
/**
 * Process request; worker (thread)
 *
 * @param queue
 */
void ProcessRequest(WorkQueue& queue, Communicator& communicator)
{
    while (g_running) {
        
        RequestData::pointer request_ptr(queue.get());
        if (request_ptr) {
             
            communicator.ProcessRequest(
                request_ptr->request_, request_ptr->connection_ptr_);
        } 
    }
}

int main(int argc, char* argv[])
{
    try{
        std::string exePath = Util::getExeDir();
        Util::setWorkingDir(exePath);
        Config::getInstance();
        const Settings settings(kDefaultSettingPath);
        /* initialize logger */       
        logger_config config;              
        logger_keeper *pKeeper = get_keeper();
        pKeeper->add_file_logger();
        pKeeper->add_console_logger();
        config.configure_with(boost::filesystem::path(kDefaultSettingPath));
        
        LOGGER_S(info)<< "Account Server Started ... ";
    
        /* intial */
        boost::thread_group thread_group;
        // create request queue & communicator thread
        WorkQueue queue;
        gorilla::account::AccountManager accountManager;
        Communicator communicator(settings, accountManager);
        for(int i = 0; i<5; i++)
        {
          thread_group.create_thread(boost::bind(ProcessRequest, boost::ref(queue), boost::ref(communicator)));      
        }
        auto baseRouter = std::make_shared<HttpRouter>();
        baseRouter->use(std::make_shared<AdapterHandler>(queue));
        HttpServer httpServer;
        if(settings.enable_https_ == 1)
        {
          httpServer.setHttps(settings.enable_https_);
          httpServer.set_cert_chain_file(settings.certificate_chain_file_path_);
          httpServer.set_private_key_file(settings.private_key_path_);
          httpServer.set_tmp_dh_file(settings.dh_file_path_);
          httpServer.set_key_password("test");
        }
        httpServer.setPort(settings.server_port_);
        httpServer.setAddress(settings.server_ip_);
        httpServer.setBaseHandler(baseRouter);
        httpServer.start();        
#ifdef WIN32
        boost::asio::signal_set signals(*httpServer.getIoServicePtr(), SIGINT, SIGTERM);
        //signals.async_wait([&httpServer, &thread_group, &queue](const boost::system::error_code& error, int signal_number) {
        signals.async_wait([](const boost::system::error_code& error, int signal_number) {
            LOGGER() << "Stopping...";
            g_running = false;
        });
        while (g_running)
        {
            std::string input;
            //std::cin.clear();//ctrl c
            std::getline(std::cin, input);
            if (std::cin.fail() || std::cin.eof() || input == "QUIT")
            {
                g_running = false;
            }
        }
        //init stop begin
        //g_running = false;
        queue.stop(); //will notify threadgroup
        thread_group.join_all();
        httpServer.stop();
        //init stop end
#else
        boost::asio::signal_set signals(*httpServer.getIoServicePtr(), SIGINT, SIGTERM);
        signals.async_wait([&httpServer, &thread_group, &queue](const boost::system::error_code& error, int signal_number){
            LOGGER() << "Stopping...";
            //init stop begin
            g_running = false;
            queue.stop(); //will notify threadgroup
            thread_group.join_all();
            httpServer.stop();
            //init stop end
        });
#endif  
        httpServer.join();
        LOGGER() << "Terminated normally";
    }
    catch(const std::exception& e){
        LOGGER() << "Abnormal termination - exception:" << e.what();
        return -1;
    }
    LOGGER()<< "Account Server Finished ... \n";
    return 0;
}
