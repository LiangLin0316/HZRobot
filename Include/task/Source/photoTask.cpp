//
// Created by Lin on 2020/8/27.
//

#include "../Header/photoTask.h"

#include "../../robot_config/Header/config.h"
#include "../../http/Header/HttpServer.h"
#include "../../http/hpp/http_client.hpp"
#include "../../mysql/Header/mysql_logs.h"
#include "./../common/action.hpp"

PhotoTask::PhotoTask(Detector *p_detector) : TaskBase(p_detector) {

}

PhotoTask::PhotoTask() {

}

PhotoTask::~PhotoTask() {

}

//前置条件检测
bool PhotoTask::checkstate() {
    return true;
}

//执行函数
int PhotoTask::execute() {

    std::cout << "正在调整机械臂高度" << std::endl;
    getchar();
    std::cout << "发送http post请求" << std::endl;
    Config *config = Config::config_instance();
    auto body = config->photo_config.json_encode();
    std::cout<<"body:"<<body<<std::endl;
    mysql_logs *mysq_log = mysql_logs::mysql_instance();

    int n_cur = 0;
    while (n_cur < config->ser_config.cn_photo && !getisvaild()) {
        n_cur++;
        try {
            shine::http::sync_client sync_c;
            sync_c.set_recv_timeout(config->ser_config.timeout);
            sync_c.get_request().set_host(config->ser_config.ea_ip+":"+config->ser_config.ea_port);
            std::cout<<config->ser_config.ea_ip+":"+config->ser_config.ea_port<<std::endl;
            sync_c.get_request().set_method(http_method_post);
            sync_c.get_request().set_content_type(http_content_json);
            sync_c.get_request().set_body(shine::string(body));
            sync_c.get_request().set_url("/request");
            if (sync_c.call()) {
                shine::string tmp;
                tmp = sync_c.get_response().get_body();
                //解析结果.等胡晓
                shine::json json;
                json.decode(tmp);
                std::cout<<json.get_root().find_kv_child("request")->find_kv_child("tablenum")->get_value()<<std::endl;
                int totalnum = atoi(json.get_root().find_kv_child("request")->find_kv_child("tablenum")->get_value());
                int successnum = atoi(json.get_root().find_kv_child("successnum")->get_value());
                if (successnum!=totalnum)
                {
                    std::cout<<"等待调整机械臂,重新拍照"<<std::endl;
                    mysq_log->recordlog(action_num_unsuccess, tmp);
                    continue;
                }
                else
                    setisvaild(true);
                mysq_log->recordlog(action_photo_success, tmp);
            } else {
                std::cout <<" 发送请求失败" << std::endl;
                mysq_log->recordlog(action_photo_faild, shine::string(body));
            }
        }
        catch (...) {
            setisvaild(false);
            mysq_log->recordlog(action_photo_faild, shine::string(body));
        }
    }
    std::cout << "机械臂高度调整完成,准备拍照" << std::endl;
    std::cout << "PhotoTask::execute" << std::endl;
    return 0;
}

//清理任务接口
void PhotoTask::clean() {
    return;
}

//最近一次执行时间
long long PhotoTask::lastrun() {
    return 0;
}


bool PhotoTask::execute_before() {
    return true;
}

