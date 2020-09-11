//
// Created by Lin on 2020/8/18.
//
#pragma once
#ifndef ROBOTAI_QUEUE_HPP
#define ROBOTAI_QUEUE_HPP

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <list>
#include <vector>

template<class T>
class CRequestQueue {
private:
    CRequestQueue() {
    }

public:
    ~CRequestQueue() {
    }

public:
    static CRequestQueue<T> *create_instance() {
        boost::mutex::scoped_lock lock(m_mutex);
        if (m_instance == NULL)
            m_instance = new CRequestQueue<T>();
        return m_instance;
    }

    void Put(const T &item) {
        boost::mutex::scoped_lock lock(m_mutex);
        m_list.push_back(item);
        m_condGet.notify_one();
    }

    void Get(T &item) {
        boost::mutex::scoped_lock lock(m_mutex);
        while (IsEmpty()) {
            m_condGet.wait(lock);
        }
        item = m_list.front();
        m_list.pop_front();
    }

    void Erase(T &item) {
        boost::mutex::scoped_lock oLock(m_mutex);
        typename std::list<T>::iterator it;
        for (it = m_list.begin(); it != m_list.end();) {
            if (item == *it) {
                m_list.erase(it++);
                break;
            } else {
                ++it;
            }
        }
    }

    bool IsEmpty() {
        return m_list.empty();
    }

    void QueueClean() {
        boost::mutex::scoped_lock oLock(m_mutex);
        if (!m_list.empty()) {
            m_list.clear();
        }

        return;
    }

    int QueueLength() {
        boost::mutex::scoped_lock oLock(m_mutex);
        return m_list.size();
    }

    T GetQueueHead(){
        if (!m_list.empty())
            return m_list.front();
        else
        {
            return nullptr;
        }
    }

private:
    std::list<T> m_list;
    static boost::mutex m_mutex;
    boost::condition_variable_any m_condGet;
    static CRequestQueue<T> *m_instance;
};

template<class T>
CRequestQueue<T> *CRequestQueue<T>::m_instance = NULL;

template<class T>
boost::mutex CRequestQueue<T>::m_mutex;

#endif //ROBOTAI_QUEUE_HPP



