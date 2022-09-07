/*
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-09-05 10:17:52
 * @LastEditTime: 2022-09-05 13:34:06
 * @Description:
 * Copyright (c) 2022 by Liyangfan.justin, All Rights Reserved. 
 */
#include "EventLoop.h"

EventLoop::EventLoop()
    :is_looping_(true),
    epoller_(new Epoll)//这里忘记初始化导致bug2
{}

void EventLoop::Loop(){
    while(is_looping_){
        std::vector<UniqChannle> returned_channels;
        returned_channels = epoller_->EpollWait();
        for(auto& it:returned_channels){
            it->HandleEvents();
        }
    }
}

void EventLoop::Quit(){
    if(is_looping_){
        is_looping_ = false;
    }
}