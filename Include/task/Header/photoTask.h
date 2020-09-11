//
// Created by Lin on 2020/8/27.
//

#ifndef ROBOTAI_PHOTOTASK_H
#define ROBOTAI_PHOTOTASK_H

#include "./taskImp.h"
#include "./detector.h"
class PhotoTask:public TaskBase
{
public:
    PhotoTask();
    PhotoTask(Detector * p_detector );
    ~PhotoTask();

public:

    bool checkstate();

    //执行函数
    int execute() ;


    //清理任务接口
    void clean();

    //最近一次执行时间
    long long lastrun();

    //前置条件
    bool execute_before();

};













#endif //ROBOTAI_PHOTOTASK_H
