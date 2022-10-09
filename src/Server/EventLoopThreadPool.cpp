/*
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-09-07 17:19:53
 * @LastEditTime: 2022-10-08 16:40:03
 * @Description: 
 * Copyright (c) 2022 by Liyangfan.justin, All Rights Reserved. 
 */
#include "EventLoopThreadPool.h"
#include "assert.h"

EventLoopThreadPool::EventLoopThreadPool(int thread_num):
    started_(true),
    thread_num_(thread_num),
    next_loop_(0)
{}

EventLoopThreadPool::~EventLoopThreadPool(){
    for(auto& loop :eventloops){
        loop->Quit();
    }
    for(auto& t:threads){
        t.get()->thread_.join();
    }
    std::cout<<"~EventLoopThreadPool"<<std::endl;
}

void EventLoopThreadPool::start(){
    started_ = true;
    for(int i=0;i<thread_num_;i++){
        std::shared_ptr<EventLoopThread> t(new EventLoopThread());
        threads.push_back(t);
        eventloops.push_back(t->GetLoop());
    }  
}

EventLoop* EventLoopThreadPool::GetNextLoop(){
    assert(started_ == true);
    assert(thread_num_>0);

    EventLoop* eventloop = eventloops[next_loop_];
    next_loop_ = (next_loop_ + 1)%thread_num_;

    return eventloop;
}