/*
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-09-07 17:16:56
 * @LastEditTime: 2022-10-08 16:41:06
 * @Description: 
 * Copyright (c) 2022 by Liyangfan.justin, All Rights Reserved. 
 */
#pragma once
#include <thread>
#include "EventLoop.h"
class EventLoopThread{
    public:
        EventLoopThread();
        ~EventLoopThread();
        EventLoop* GetLoop();
        std::thread thread_;
    private:
        // std::thread thread_;
        void ThreadFunc();
        EventLoop* eventloop_;
};