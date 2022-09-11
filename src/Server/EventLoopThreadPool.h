/*
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-09-07 17:19:41
 * @LastEditTime: 2022-09-07 20:35:51
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
        ~EventLoopThreadPool() = default;
        void start();
        EventLoop* GetNextLoop();

    private:
        bool started_;
        int thread_num_;
        int next_loop_;
        std::vector<std::shared_ptr<EventLoopThread>> threads;
        std::vector<EventLoop*> eventloops;
};