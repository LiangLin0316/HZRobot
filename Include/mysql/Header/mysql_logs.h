//
// Created by Lin on 2020/9/8.
//
#pragma once
#ifndef ROBOTAI_MYSQL_LOGS_H
#define ROBOTAI_MYSQL_LOGS_H

#include <stdio.h>
#include <time.h>
#include "../mysql.hpp"
#include "config.h"
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

class mysql_logs {

private:
    mysql_logs() {
        Config *config = Config::config_instance();
        config->loadmysql_config();
        connect_info.set_addr(config->sql_config.ip + ":" + config->sql_config.port);
        connect_info.set_user(config->sql_config.user);
        connect_info.set_password(config->sql_config.passwd);
        connect_info.set_database(config->sql_config.dbname);
        std::cout << config->sql_config.ip << " " << config->sql_config.user << " " << config->sql_config.passwd << " "
                  << config->sql_config.dbname;
        pool.pool_init(connect_info);
        conn = pool.get();

    }

public:
    static mysql_logs *mysql_instance() {
        if (m_instance == NULL) {
            m_instance = new mysql_logs();
        }
        return m_instance;
    }

    //write logs
    bool recordlog(string action,  string remarks = "") {
        boost::mutex::scoped_lock oLock(m_mutex);
        char res[100];
        memset(res, 0, sizeof(res));
        time_t now;
        struct tm *tm_now;
        time(&now);
        tm_now = localtime(&now);
        sprintf(res,"%d-%d-%d %d:%d:%d", tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday, tm_now->tm_hour,
               tm_now->tm_min, tm_now->tm_sec);
        std::string s_date(res);
        shine::string sql = "INSERT INTO `run_logs` (`date`, `action`, `remarks`) VALUES ";
        sql.format_append("('%s', '%s', '%s') ", s_date.c_str(), action.c_str(), remarks.c_str());
        return conn->execute(sql);
    }

private:
    static mysql_logs *m_instance;
    static boost::mutex m_mutex;


public:
    std::shared_ptr<shine::db::mysql> conn;
    shine::db::mysql_connect_info connect_info;
    shine::db::mysql_pool pool;
};
#endif
