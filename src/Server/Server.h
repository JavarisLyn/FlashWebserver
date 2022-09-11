/*
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-09-05 11:00:30
 * @LastEditTime: 2022-09-07 21:39:28
 * @Description: 
 * Copyright (c) 2022 by Liyangfan.justin, All Rights Reserved. 
 */
#pragma once
#include "Utils.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"
#include <iostream>
class Server{
    
    public:
        Server(EventLoop * event_loop,int port,int thread_num);
        ~Server() = default;
        void start();
        

    private:
        int listen_fd_;
        int port_;
        int thread_num_;
        EventLoop* event_loop_;
        std::unique_ptr<EventLoopThreadPool> eventLoopThreadPool_;

    private:
        void HandleNewConn();
};