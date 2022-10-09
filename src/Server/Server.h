/*
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-09-05 11:00:30
 * @LastEditTime: 2022-10-08 16:28:58
 * @Description: 
 * Copyright (c) 2022 by Liyangfan.justin, All Rights Reserved. 
 */
#pragma once
#include "../Utils/Utils.h"
#include "Http.h"
#include "EventLoopThreadPool.h"
#include <iostream>
class Server{
    
    public:
        Server(EventLoop * event_loop,int port,int thread_num);
        ~Server() = default;
        void start();
        void shutdown();
        

    private:
        int listen_fd_;
        int port_;
        int thread_num_;
        EventLoop* event_loop_;
        std::unique_ptr<EventLoopThreadPool> eventLoopThreadPool_;

    private:
        void HandleNewConn();
};