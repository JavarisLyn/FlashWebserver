/*
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-09-01 16:59:45
 * @LastEditTime: 2022-09-05 11:04:40
 * @Description: 
 * Copyright (c) 2022 by Liyangfan.justin, All Rights Reserved. 
 */
#pragma once
#include <memory>
#include "Epoll.h"
#include "Channel.h"

class EventLoop{
    public:
        EventLoop();
        ~EventLoop();

        void Loop();
        void Quit();

        void AddToEpoller(UniqChannle channel){
            epoller_->EpollAdd(std::move(channel));
        }

    private:
        /* 控制loop启停 */
        bool is_looping_;
        /* 无需使用shared_ptr */
        std::unique_ptr<Epoll> epoller_;

};