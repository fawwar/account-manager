#pragma once
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <functional>

class JobManager
{
    boost::asio::io_service ioService;
    boost::thread_group threadPool;
    boost::asio::io_service::work work;
    
    JobManager();
    ~JobManager();
    
  public:
    static JobManager& GetInstance();
    void doJob(std::function<void(void)> f);
};
