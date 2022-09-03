/*
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-09-01 16:56:44
 * @LastEditTime: 2022-09-03 16:19:32
 * @Description: 一个Channel负责处理一个fd,属于一个EventLoop
 * Copyright (c) 2022 by Liyangfan.justin, All Rights Reserved. 
 */
#pragma once
#include <functional>
#include <sys/epoll.h>

/* 前向声明，因为Channel包含Eventloop，同时Eventloop也包含Channel，include会导致循环引用 */
/* https://www.jianshu.com/p/9e247c805d5a */
class EventLoop;

class Channel{

private:

    typedef std::function<void()> Callback;

    /* 处理的fd */
    int fd_;

    /* 所属的EventLoop */
    EventLoop* eventloop_;

    /* 活跃的事件 */
    __uint32_t active_events_;

    /* 对应事件的回调函数 */
    Callback read_callback_;
    Callback write_callback_;
    Callback error_callback_;
    Callback conn_callback_;
    
public:

    Channel(EventLoop* eventLoop,int fd);
    ~Channel();

    int Getfd();
    void Setfd(int fd);

    /* 这里比较复杂，牵扯到左值、右值、万能引用、函数模板 */
    void SetReadCallback(Callback read_callback);
    void SetWriteCallback(Callback write_callback);
    void SetErrorCallback(Callback error_callback);
    void SetConnCallback(Callback conn_callback);

private:
    /* 处理监听到的事件 */
    void HandleEvents();
    void HandleRead();
    void HandleWrite();
    void HandleError();
    void HandleConn();

    void SetActiveEvents();

};