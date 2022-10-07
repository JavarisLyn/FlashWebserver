/*
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-10-06 10:08:50
 * @LastEditTime: 2022-10-07 12:34:28
 * @Description: 
 * Copyright (c) 2022 by Liyangfan.justin, All Rights Reserved. 
 */
#include "Timer.h"
#include <sys/time.h>
//define a member functino out of class should start with `template`
TimerNode::TimerNode(std::shared_ptr<Http> data, int timeout):
    data_(data),
    is_deleted_(false){
        struct timeval now;
        gettimeofday(&now,nullptr);
        expire_time_ = (now.tv_sec % 1000) * 1000 + (now.tv_usec / 1000) + timeout;
        //std::cout<<"timer node created"<<std::endl;
    }

TimerNode::~TimerNode(){
    /* 
        if update time,then data_ is nullptr,which means handleClose won't be called.
        if expired,data_ is not null,handleClose will be called.
     */
    if(data_){
        //then http object will call deconstruct
       data_->HandleClose();
    }
    //std::cout<<"timer node deconstruct"<<std::endl;
}

void TimerNode::Update(int timeout){
    struct timeval now;
    gettimeofday(&now,nullptr);
    expire_time_ = (now.tv_sec % 1000) * 1000 + (now.tv_usec / 1000) + timeout;
}

bool TimerNode::IsExpired(){
    //std::cout<<"check isexpired"<<std::endl;
    struct timeval now;
    gettimeofday(&now, nullptr);
    size_t nowms = (((now.tv_sec % 1000) * 1000) + (now.tv_usec / 1000));
    //std::cout<<"now:"<<nowms<<"timeout:"<<expire_time_<<std::endl;
    if(nowms < expire_time_){
        return false;
    }else{
        SetDeleted();
        return true;
    }
}

void TimerNode::ClearNode(){
    //this shared_ptr won't point to it's data any more
    data_.reset();
    SetDeleted();
}

void TimerManager::AddTimerNode(std::shared_ptr<Http> data,int timeout){
    std::shared_ptr<TimerNode> timer_node(new TimerNode(data,timeout));
    timer_node_queue_.push(timer_node);
    data->LinkTimer(timer_node);
    //std::cout<<"add timer node finish"<<std::endl;
}

void TimerManager::HandleExpire(){
   // std::cout<<"HandleExpire"<<std::endl;
   while(!timer_node_queue_.empty()){
       std::shared_ptr<TimerNode> timer_node = timer_node_queue_.top();
       if(timer_node->IsExpired() || timer_node->IsDeleted()){
           //std::cout<<"timer node pop"<<std::endl;
           timer_node_queue_.pop();
       }else{
           break;
       }
   }
}

