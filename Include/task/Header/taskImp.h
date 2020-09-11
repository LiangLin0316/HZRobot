//
// Created by Lin on 2020/8/27.
//

#ifndef ROBOTAI_TASKIMP_H
#define ROBOTAI_TASKIMP_H


#include "./detector.h"
#include<iostream>
class TaskBase {
public:
    TaskBase();

    TaskBase(Detector *detector_base);

    virtual ~TaskBase();

    virtual int execute() ;


    virtual void clean();

    virtual long long lastrun();

    virtual bool checkstate();

    //执行前置条件
    virtual bool execute_before();
    //执行后置条件
    virtual bool execute_after();

    //检测是否执行成功
    bool getisvaild();

    //设置是否可行
    void setisvaild(bool flag);
private:
    Detector *detector;
    bool finish;

};

#endif //ROBOTAI_TASKIMP_H