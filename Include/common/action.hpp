//
// Created by Lin on 2020/9/9.
//
#pragma once
#ifndef ROBOTAI_ACTIONDEFINE_HPP
#define ROBOTAI_ACTIONDEFINE_HPP


#define action_begin "启动程序"
#define action_init "程序初始化"
#define action_task "启动任务处理线程"
#define action_web "启动WEB服务线程"
#define action_run_get "任务队列等待任务获取"
#define action_run_finish "当前任务执行完成"
#define action_photo "拍照"
#define action_post "收到POST请求"
#define action_get "收到GET请求"
#define action_continue "继续运行"
#define action_photo_faild "拍照任务执行失败"
#define action_photo_success "拍照任务执行成功"
#define action_continhue_success "成功发送继续执行给机器人"
#define action_continhue_faild "发送继续执行给机器人失败"
#define action_num_unsuccess "拍照未执行成功,调整后重新拍照"


//拍照
#define run_photo 1
//拾音
#define run_pick_sound 2
//测温
#define run_pick_temperature 3

#endif //ROBOTAI_ACTIONDEFINE_HPP
