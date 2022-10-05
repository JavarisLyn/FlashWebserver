/*
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-09-03 15:55:03
 * @LastEditTime: 2022-10-03 20:48:06
 * @Description: 
 * Copyright (c) 2022 by Liyangfan.justin, All Rights Reserved. 
 */
#include "Channel.h"
#include <iostream>
#include <unistd.h>

Channel::Channel(EventLoop *eventloop,int fd)
    :eventloop_(eventloop),
    fd_(fd),
    to_listen_events_((__uint32_t)0)
{
    LOG_TRACE("Channel construct,fd:%d\n",fd_);
}

Channel::~Channel(){
    // close(fd_);
    LOG_TRACE("Channel deconstruct,fd:%d\n",fd_);
    // std::cout<<"Chanel deconstrcut"<<std::endl;
}

int Channel::Getfd(){
    return fd_;
}

/* 非构造函数不能使用成员初值列 */
void Channel::Setfd(int fd){
    fd_ = fd;
}

__uint32_t Channel::GetToListenEvents(){
    return to_listen_events_;
}

void Channel::SetToListenEvents(__uint32_t new_events){
    to_listen_events_ = new_events;
}

void Channel::SetActiveEvents(__uint32_t active_events){
    active_events_ = active_events;
}

void Channel::SetReadCallback(Callback read_callback){
    read_callback_ = read_callback;
}

void Channel::SetWriteCallback(Callback write_callback){
    write_callback_ = write_callback;
}

void Channel::SetErrorCallback(Callback error_callback){
    error_callback_ = error_callback;
}

void Channel::SetConnCallback(Callback conn_callback){
    conn_callback_ = conn_callback;
}

void Channel::HandleRead(){
    if(read_callback_){
        read_callback_();
    }
}

void Channel::HandleWrite(){
    if(write_callback_){
        write_callback_();
    }
}

void Channel::HandleError(){
    if(error_callback_){
        error_callback_();
    }
}

void Channel::HandleConn(){
    if(conn_callback_){
        conn_callback_();
    }
}

void Channel::HandleEvents(){
    if((active_events_ & EPOLLHUP) && !(active_events_ & EPOLLIN)){
        return ;
    }
    if(active_events_ & EPOLLOUT){
        HandleWrite();
    }
    if(active_events_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)){
        // std::cout<<"handle read"<<std::endl;
        HandleRead();
    }
    if(active_events_ & EPOLLERR){
        HandleError();
    }
    /* handleconn todo */
    HandleConn();
    // std::cout<<"new coon"<<std::endl;
}



