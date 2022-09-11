/*
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-09-05 10:17:52
 * @LastEditTime: 2022-09-07 16:51:18
 * @Description:
 * Copyright (c) 2022 by Liyangfan.justin, All Rights Reserved. 
 */
#include "EventLoop.h"
#include "Utils.h"
EventLoop::EventLoop()
    :is_looping_(true),
    epoller_(new Epoll),//这里忘记初始化导致bug2
    running_callbacks_(false),
    /* 宏定义不需要加namespace,eg Utils::gettid,但是方法需要加 */
    thread_id_(gettid())
{}

void EventLoop::Loop(){
    while(is_looping_){
        std::vector<UniqChannle> returned_channels;
        returned_channels = epoller_->EpollWait();
        for(auto& it:returned_channels){
            it->HandleEvents();
        }
        /* 开始执行回调函数 */
        DoCallbacks();
    }
}

void EventLoop::DoCallbacks(){
    running_callbacks_ = true;
    /* 拷贝后执行，减少对callback_list_mutex的锁持有时间 */
    std::vector<Callback> to_copy_list;
    //RAII
    {
        //lock
        std::unique_lock<std::mutex> lck(callback_list_mutex_);
        to_copy_list.swap(callback_list_);
    }
    
    for(auto& t:to_copy_list){
        t();
    }
    running_callbacks_ = false;
}

void EventLoop::RunFunction(Callback&& cb){
    if(IsInLoopthread()){
        cb();
    }else{
        QueueINCallback(std::move(cb));
    }
}

bool EventLoop::IsInLoopthread() const{
    return thread_id_ == gettid();
}

void EventLoop::QueueINCallback(Callback&& cb){
    /* RAII */
    {
        std::unique_lock<std::mutex> lck(callback_list_mutex_);
        callback_list_.emplace_back(std::move(cb));
    }
    /* 非本线程且目标线程未在执行回调，唤醒 */
    if(!IsInLoopthread() && running_callbacks_){
        WakeUpTargetEventLoop();
    }
   
}

/* 唤醒epoll返回 */
void EventLoop::WakeUpTargetEventLoop(){

}

void EventLoop::Quit(){
    if(is_looping_){
        is_looping_ = false;
    }
}