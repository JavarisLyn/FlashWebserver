/*
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-09-07 17:16:56
 * @LastEditTime: 2022-09-07 20:41:01
 * @Description: 
 * Copyright (c) 2022 by Liyangfan.justin, All Rights Reserved. 
 */
#pragma once
#include <thread>
#include "EventLoop.h"
class EventLoopThread{
    public:
        EventLoopThread();
        ~EventLoopThread() = default;
        EventLoop* GetLoop();
    private:
        std::thread thread_;
        void ThreadFunc();
        EventLoop* eventloop_;
};