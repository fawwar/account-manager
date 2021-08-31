#pragma once
#include <cstdlib>
#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T> class FixedSizeQueue
{
public:
    FixedSizeQueue(const int max_size);
    ~FixedSizeQueue();
    T Dequeue();
    T Dequeue(int timeout);
    int Enqueue(T& item);
    int ForceEnqueue(T& item);
    int Size();

private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_condition;
    int m_max_size;
};

template <typename T> FixedSizeQueue<T>::FixedSizeQueue(const int max_size)
    : m_max_size(max_size)
{
}

template <typename T> FixedSizeQueue<T>::~FixedSizeQueue() {}

template <typename T> T FixedSizeQueue<T>::Dequeue()
{
    std::unique_lock<std::mutex> m_unique_lock(m_mutex);
    while (m_queue.empty()) {
        m_condition.wait(m_unique_lock);
    }
    auto item = std::move(m_queue.front());
    m_queue.pop();

    return item;
}

template <typename T> T FixedSizeQueue<T>::Dequeue(int timeout)
{
    std::unique_lock<std::mutex> m_unique_lock(m_mutex);
    while (m_queue.empty()) {
        if (m_condition.wait_for(m_unique_lock, std::chrono::milliseconds(timeout))
            == std::cv_status::timeout) {
            return NULL;
        }
    }
    auto item = std::move(m_queue.front());
    m_queue.pop();

    return item;
}

template <typename T> int FixedSizeQueue<T>::Enqueue(T& item)
{
    std::unique_lock<std::mutex> m_unique_lock(m_mutex);
    if (m_queue.size() <= m_max_size) {
        m_queue.push(std::move(item));
    } else {
        return EXIT_FAILURE;
    }
    m_condition.notify_one();

    return EXIT_SUCCESS;
}

template <typename T> int FixedSizeQueue<T>::ForceEnqueue(T& item)
{
    std::unique_lock<std::mutex> m_unique_lock(m_mutex);
    if (m_queue.size() <= m_max_size) {
        m_queue.push(std::move(item));
        m_condition.notify_one();
        return EXIT_SUCCESS;
    } else {
        m_queue.pop();
        m_queue.push(std::move(item));
        m_condition.notify_one();
        return EXIT_FAILURE;
    }

}

template <typename T> int FixedSizeQueue<T>::Size()
{
    return m_queue.size();
}

