/*
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-09-07 17:17:05
 * @LastEditTime: 2022-09-11 20:47:32
 * @Description: 
 * Copyright (c) 2022 by Liyangfan.justin, All Rights Reserved. 
 */
#include "EventLoopThread.h"
#include <functional>

EventLoopThread::EventLoopThread():
    eventloop_(nullptr)
{
    std::thread t(&EventLoopThread::ThreadFunc,this);
    thread_ = std::move(t);
} 

EventLoop* EventLoopThread::GetLoop(){
    /* eventloop_是异步创建,需要阻塞等待eventloop_初始化完毕 */
    // TODO 用锁完善
    while(eventloop_ == nullptr){
        
    }
    return eventloop_;
}

void EventLoopThread::ThreadFunc(){
    /* 在这里初始化EventLoop */
    EventLoop eventloop;
    eventloop_ = &eventloop;
    eventloop_->Loop();
    eventloop_ = nullptr;
}