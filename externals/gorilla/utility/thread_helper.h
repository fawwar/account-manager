#pragma once
#include "debug.h"
#include <iostream>
#include <atomic>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <utility>
#include <chrono>
#include <functional>
#include <algorithm>
#include <sys/eventfd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

class ThreadCommandBase
{
public:
    virtual void Finish() { delete this; }

    virtual void WaitFinish()
    {
        // TODO
    }
};

template <typename T> class SafeQueue
{
public:
    SafeQueue()
        : m_stop(false)
    {
    }

    T GetHead()
    {
        std::lock_guard<std::mutex> queue_lock(m_mutex);
        if (m_queue.empty()) {
            return NULL;
        }

        auto item = m_queue.front();
        m_queue.pop();

        return item;
    }

    bool Dequeue(T& item, int timeoutMS = 10000)
    {
        bool ret = true;
        {
            std::unique_lock<std::mutex> queue_lock(m_mutex);

            if (m_queue.empty()) {
                if (m_condition.wait_for(
                        queue_lock, std::chrono::milliseconds(timeoutMS))
                    == std::cv_status::timeout) {
                    ret = false;
                }
            }
        }

        if (ret) {
            item = GetHead();
        }

        return ret;
    }

    bool Enqueue(T& item)
    {
        {
            std::lock_guard<std::mutex> queue_lock(m_mutex);
            if (m_stop) {
                return false;
            }
            m_queue.push(item);
        }
        m_condition.notify_one();

        return true;
    }

    void StopEnqueue(bool stop)
    {
        std::lock_guard<std::mutex> queue_lock(m_mutex);
        m_stop = stop;
    }

private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_condition;
    bool m_stop;
};

template <typename T_EVT, typename T_CMD> int EventLoop(void* p);

template <typename T_EVT, typename T_CMD> class ThreadHelper
{
public:
    ThreadHelper()
        : m_run(false)
        , m_pThread(NULL)
    {
        for (int i = 0; i < T_EVT::_EVENT_TOTAL; ++i) {
            m_event_fds[i] = -1;
        }
    }

    virtual ~ThreadHelper()
    {
        StopThread();

        for (int i = 0; i < T_EVT::_EVENT_TOTAL; ++i) {
            if (m_event_fds[i] != -1) {
                close(m_event_fds[i]);
                m_event_fds[i] = -1;
            }
        }
    };

    virtual void OnCommand(void){};
    virtual void OnEvent(int){};

    virtual void InitInstance(void){};
    virtual void ExitInstance(void){};

    void StartThread(void)
    {
        m_run = true;
        m_pThread = new std::thread(EventLoop<T_EVT, T_CMD>, this);
    }

    void StopThread(void)
    {
        m_run = false;
        if (m_pThread != NULL) {
            AddEvent(T_EVT::EVENT_QUIT);

            m_pThread->join();
            m_pThread = NULL;
        }
    }

    virtual void OnEventQuit(){};

    int AddCommand(T_CMD* ptr_cmd)
    {
        m_commandQueue.Enqueue(ptr_cmd);
        AddEvent(T_EVT::EVENT_CMD);
    }

    /* NOTE: DON'T add events in commands. Because it's still in the same
     * select loop, the select won't triggler.
     */
    int AddEvent(int evtType)
    {
        if (m_event_fds[evtType] != -1) {
#ifdef __arm__
            eventfd_write(m_event_fds[evtType], 1);
#else
            size_t i = 1;
            if (write(m_event_fds[evtType], &i, sizeof(size_t)) == -1)
                DBG_ERR("AddEvent failed");
#endif
        }
    }

    int AddWaitingEvent(int evtType)
    {
        int fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
        m_event_fds[evtType] = fd;
    }

public:
    bool m_run = true;
    int m_event_fds[T_EVT::_EVENT_TOTAL];

protected:
    SafeQueue<T_CMD*> m_commandQueue;
    std::thread* m_pThread;
};

template <typename T_EVT, typename T_CMD> int EventLoop(void* p)
{
    ThreadHelper<T_EVT, T_CMD>* parent = (ThreadHelper<T_EVT, T_CMD>*)p;

    // init
    parent->InitInstance();

    while (parent->m_run) {
        int ret;
        int max_fd = *std::max_element(parent->m_event_fds,
                         parent->m_event_fds + T_EVT::_EVENT_TOTAL);
        fd_set wait_fd_set;

        FD_ZERO(&wait_fd_set);
        for (int i = 0; i < T_EVT::_EVENT_TOTAL; i++) {
            if (parent->m_event_fds[i] != -1) {
                FD_SET(parent->m_event_fds[i], &wait_fd_set);
            }
        }

        ret = select(max_fd + 1, &wait_fd_set, NULL, NULL, NULL);
        if (ret < 0) {
            DBG_ERR("Select error");
            continue;
        } else if (ret == 0) {
            DBG_ERR("Select timeout");
        } else {
            int i = 0;
#ifdef __arm__
            eventfd_t u = 0;
#else
            size_t u = 0;
#endif
            for (i = 0; i < T_EVT::_EVENT_TOTAL; ++i) {
                if (parent->m_event_fds[i] != -1) {
                    if (FD_ISSET(parent->m_event_fds[i], &wait_fd_set)) {
#ifdef __arm__
                        if (eventfd_read(parent->m_event_fds[i], &u) != -1) {
#else
                        if (read(parent->m_event_fds[i], &u, sizeof(size_t))
                            > 0) {
#endif
                            while (u) {
                                --u;
                                parent->OnEvent(i);
                            }
                        }
                    }
                }
            }
        }
    }

    // exit
    parent->ExitInstance();
}
