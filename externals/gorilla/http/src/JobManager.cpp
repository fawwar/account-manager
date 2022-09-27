#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include "JobManager.h"

JobManager::JobManager():
    work(ioService)
{
    threadPool.create_thread(boost::bind(&boost::asio::io_service::run, &ioService));
}

JobManager::~JobManager()
{
    ioService.stop();
    threadPool.join_all();
}

JobManager& JobManager::GetInstance()
{
    static JobManager m;
    return m;
}

void JobManager::doJob(std::function<void(void)> f)
{
    ioService.post(f);
}

