//
// Created by Lin on 2020/8/27.
//
#include"taskImp.h"
#include "./../common/action.hpp"
#include "../../robot_config/Header/config.h"
#include "../../http/hpp/http_client.hpp"
#include "../../mysql/Header/mysql_logs.h"

TaskBase::TaskBase()
{
    finish = false;
    detector= nullptr;
}
TaskBase::TaskBase(Detector *detector_base) {
    detector = detector_base;
    finish = false;
}

TaskBase::~TaskBase() {
}

void TaskBase::clean() {
}

long long TaskBase::lastrun() {
    return 0;
}

bool TaskBase::checkstate()
{
    if(nullptr == detector)
        return false;
    return detector->isArrive();
}



//执行函数
int TaskBase::execute()
{
    return 0;
}

//检测是否执行成功
bool TaskBase::getisvaild()
{
    return finish;
}

//设置是否可行
void TaskBase::setisvaild(bool flag)
{

     finish = flag;
}

 bool TaskBase::execute_before()
 {
     return true;
 }
//执行后置条件
 bool TaskBase::execute_after()
{
    //continue run
    Config *config = Config::config_instance();
    auto body = config->c_robot_ip.json_encode();
    mysql_logs *mysq_log = mysql_logs::mysql_instance();
    shine::http::sync_client sync_c;
    sync_c.set_recv_timeout(config->ser_config.timeout);
    sync_c.get_request().set_host(config->ser_config.ea_ip+config->ser_config.ea_port);
    sync_c.get_request().set_method(http_method_post);
    sync_c.get_request().set_content_type(http_content_json);
    sync_c.get_request().set_body(shine::string(body));
    sync_c.get_request().set_url("http://192.168.1.235:8080/api/v2/vehicles/missionWork/continue");
    bool res_flag = false;
    int n_cur = 0;
    while (n_cur < config->ser_config.cn_photo && !res_flag) {
        n_cur++;
        try {
            if (sync_c.call()) {
                res_flag  = true;
                mysq_log->recordlog(action_continhue_success);
            } else {
                mysq_log->recordlog(action_continhue_faild, shine::string(body));
            }
        }
        catch (...) {
            res_flag = false;
            mysq_log->recordlog(action_photo_faild, shine::string(body));
        }
    }
    return true;
}