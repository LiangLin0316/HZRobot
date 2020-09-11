//
// Created by Lin on 2020/9/9.
//

#include "../Header/mysql_logs.h"
mysql_logs *mysql_logs::m_instance = NULL;
boost::mutex mysql_logs::m_mutex;
