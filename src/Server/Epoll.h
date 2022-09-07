/*
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-09-01 16:51:02
 * @LastEditTime: 2022-09-05 16:59:40
 * @Description: 
 * Copyright (c) 2022 by Liyangfan.justin, All Rights Reserved. 
 */
#pragma once
#include <vector>
#include <unordered_map>
#include <sys/epoll.h>
#include <iostream>
#include "Channel.h"
#include <memory>

typedef std::unique_ptr<Channel> UniqChannle;

class Epoll{

    public:
        Epoll();
        ~Epoll();
        void EpollAdd(UniqChannle channel);
        void EpollModify(UniqChannle channel);
        void EpollDel(UniqChannle channel);
        std::vector<UniqChannle> EpollWait();
        /* todo 调用会多次拷贝 */
        std::vector<epoll_event> GetReturnedFdEvents(){
            return returned_fd_events_;
        }
        std::vector<int> to_listen_fds_;


    private:
        const int MAXFDS_ = 10000;
        /* 一个fd监听的最多的event数量 */
        const int MAXEVENTS = 4096;
        /* 10s */
        const int EPOLLWAIT_TIMEOUT = 10000;
        int epoll_fd_;
        // std::vector<epoll_event> returned_fd_events_;
        std::unordered_map<int,UniqChannle> fd2channel_;

        std::vector<epoll_event> returned_fd_events_;

        
};
