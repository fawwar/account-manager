#pragma once
#include <http/Lib.h>
#include <atomic>
#include <list>
#include <mutex>
#include <condition_variable>

using namespace gorilla::http;
typedef Lib::HttpServer Server;
extern std::atomic<bool> g_running;

/**
 * request + connection encapsulation (work item)
 */
struct RequestData {
    const Lib::HttpRequest request_;
    Lib::HttpConnection connection_ptr_;

    typedef std::shared_ptr<RequestData> pointer;

    RequestData(const Lib::HttpRequest& req, Lib::HttpConnection conn)
        : request_(req)
        , connection_ptr_(conn)
    {
    }
};


/**
 * A basic work queue
 */
struct WorkQueue {
    std::list<RequestData::pointer> list_;
    std::mutex mutex_;
    std::condition_variable m_condition;
    
//    std::unique_lock<std::mutex> m_unique_lock(m_mutex);
    inline void put(const RequestData::pointer& request_data_ptr)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        list_.push_back(request_data_ptr);
        m_condition.notify_one();
    }
    
    inline void stop()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        list_.clear();
        m_condition.notify_all();
    }

    inline RequestData::pointer get()
    {
        if(!g_running) return nullptr;
        
        std::unique_lock<std::mutex> lock(mutex_);
        if (list_.empty())
        {
            m_condition.wait(lock);
        }

        RequestData::pointer request_data_ptr;
        if (!list_.empty()) {
            request_data_ptr = list_.front();
            list_.pop_front();
        }

        return request_data_ptr;
    }
};
