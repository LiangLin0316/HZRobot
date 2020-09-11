//
// Created by Lin on 2020/8/26.
//
#pragma once
#ifndef ROBOTAI_TASKMANGER_H
#define ROBOTAI_TASKMANGER_H
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "../../robot_config/Header/config.h"
#include "../../mysql/Header/mysql_logs.h"
#include "../../queue/queue.hpp"
#include "../Header/taskImp.h"
#include "../Header/detector.h"
#include "../../common/action.hpp"
#include <iostream>
#include <thread>

class TaskManger
{
public:
    ~TaskManger()
    {

    }

private:
    TaskManger()
    {
        std::cout<<"调用job构造函数"<<std::endl;
        job = CRequestQueue<TaskBase*>::create_instance();
    }

public:
    static TaskManger* get_instance()
    {
        boost::mutex::scoped_lock oLock(t_mutex);
        if(NULL == task_instance)
        {
            task_instance = new TaskManger();
        }
        return task_instance;
    }

    void taskPoll()
    {
        //double lock
        boost::mutex::scoped_lock oLock(task_mutex);
        job = CRequestQueue<TaskBase*>::create_instance();
        mysql_logs *mysq_log = mysql_logs::mysql_instance();
        while(true)
        {
            TaskBase *taskBase = nullptr;
            //获取任务
            mysq_log->recordlog(action_run_get);
            job->Get(taskBase);
            if(taskBase!= nullptr)
            {
                std::cout<<"执行任务"<<std::endl;
                taskBase->execute();
                if(taskBase->getisvaild())
                {
                    taskBase->execute_after();
                }
                delete taskBase;
            }
            mysq_log->recordlog(action_run_finish);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }

    void taskpush(TaskBase *taskBase)
    {
        boost::mutex::scoped_lock oLock(t_mutex);
        job = CRequestQueue<TaskBase*>::create_instance();
        job->Put(taskBase);
    }

    //start del jobs
    void start_del( )
    {
        boost::mutex::scoped_lock oLock(t_mutex);
        boost::thread* thrd_del = new boost::thread(boost::bind(&TaskManger::taskPoll,this));
        thrd_del->detach();
    }

    //put jobs
    void start_product(TaskBase* taskBase)
    {
        boost::mutex::scoped_lock oLock(t_mutex);
        boost::thread* thrd_prod = new boost::thread(boost::bind(&TaskManger::taskpush,this,taskBase));
        thrd_prod->detach();
    }

private:
    static TaskManger* task_instance;
    static boost::mutex t_mutex;
    static boost::mutex task_mutex;
public:
    CRequestQueue<TaskBase* > *job;
};

#endif //ROBOTAI_TASKMANGER_H
