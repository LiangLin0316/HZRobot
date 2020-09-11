//
// Created by Lin on 2020/8/26.
//

#include "../Header/taskManger.h"

TaskManger* TaskManger::task_instance = NULL;
boost::mutex TaskManger::t_mutex;
boost::mutex TaskManger::task_mutex;
