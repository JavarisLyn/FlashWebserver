/*
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-09-05 10:17:52
 * @LastEditTime: 2022-10-03 20:48:43
 * @Description:
 * Copyright (c) 2022 by Liyangfan.justin, All Rights Reserved. 
 */
#include "EventLoop.h"
#include "Utils.h"
#include <sys/eventfd.h>
EventLoop::EventLoop()
    :is_looping_(true),
    epoller_(new Epoll),//这里忘记初始化导致bug2
    running_callbacks_(false),
    /* 宏定义不需要加namespace,eg Utils::gettid,但是方法需要加 */
    thread_id_(gettid()),
    wakeup_fd_(CreateEventFd()),
    wakeup_channel_(new Channel(this,wakeup_fd_))
{
    /* 注册wakeup_channel到epoller */
    wakeup_channel_->SetToListenEvents(EPOLLIN | EPOLLET);
    // wakeup_channel_->SetReadCallback(bind(&EventLoop::handleRead, this));
    // wakeup_channel_->SetConnCallback(bind(&EventLoop::handleConn, this));
    epoller_->EpollAdd(wakeup_channel_);
}

EventLoop:: ~EventLoop(){
    close(wakeup_fd_);
    // std::cout<<"Eventloop deconstruct"<<std::endl;
}

void EventLoop::Loop(){
    while(is_looping_){
        std::vector<SharedChannel> returned_channels;
        returned_channels = epoller_->EpollWait();
        for(auto& it:returned_channels){
            it->HandleEvents();
        }
        /* 开始执行回调函数 */
        DoCallbacks();
    }
}

void EventLoop::DoCallbacks(){
    // std::cout<<"start doing callbacks"<<std::endl;
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

void EventLoop::RunFunction(Callback cb){
    if(IsInLoopthread()){
        cb();
    }else{
        QueueINCallback(cb);
    }
}

bool EventLoop::IsInLoopthread() const{
    return thread_id_ == gettid();
}

void EventLoop::QueueINCallback(Callback cb){
    /* RAII */
    {
        std::unique_lock<std::mutex> lck(callback_list_mutex_);
        callback_list_.emplace_back(std::move(cb));
    }

    // if(!IsInLoopthread() || running_callbacks_){
    //     WakeUpTargetEventLoop();
    // }
    WakeUpTargetEventLoop();
}

int EventLoop::CreateEventFd(){
    //#include <sys/eventfd.h>
    int event_fd = eventfd(0,EFD_NONBLOCK | EFD_CLOEXEC);
    if(event_fd<0){
        //TODO 统一异常捕获
        perror("event fd create failed");
    }
    return event_fd;
}

/* 唤醒epoll返回 */
void EventLoop::WakeUpTargetEventLoop(){
    uint64_t data = 1;
    Utils::WriteToFd(wakeup_fd_,&data,sizeof data);
    // std::cout<<"wake up fd"<<std::endl;
}

void EventLoop::Quit(){
    if(is_looping_){
        is_looping_ = false;
    }
}