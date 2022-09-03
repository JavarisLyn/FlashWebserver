/*
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-09-03 15:55:03
 * @LastEditTime: 2022-09-03 16:35:15
 * @Description: 
 * Copyright (c) 2022 by Liyangfan.justin, All Rights Reserved. 
 */
#include "Channel.h"


Channel::Channel(EventLoop *eventloop,int fd)
    :eventloop_(eventloop),
    fd_(fd)
{}

int Channel::Getfd(){
    return fd_;
}

/* 非构造函数不能使用成员初值列 */
void Channel::Setfd(int fd){
    fd_ = fd;
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
        HandleRead();
    }
    if(active_events_ & EPOLLERR){
        HandleError();
    }
    /* handleconn todo */
}



