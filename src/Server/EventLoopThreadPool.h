/*
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-09-07 17:19:41
 * @LastEditTime: 2022-10-08 16:30:24
 * @Description: 
 * Copyright (c) 2022 by Liyangfan.justin, All Rights Reserved. 
 */
#pragma once
#include "EventLoop.h"
#include "EventLoopThread.h"
#include <vector>
#include <memory>
class EventLoopThreadPool{
    public:
        EventLoopThreadPool(int thread_num);
        ~EventLoopThreadPool();
        void start();
        EventLoop* GetNextLoop();

    private:
        bool started_;
        int thread_num_;
        int next_loop_;
        std::vector<std::shared_ptr<EventLoopThread>> threads;
        std::vector<EventLoop*> eventloops;
};