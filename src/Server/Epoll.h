/*
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-09-01 16:51:02
 * @LastEditTime: 2022-10-07 10:20:39
 * @Description: 
 * Copyright (c) 2022 by Liyangfan.justin, All Rights Reserved. 
 */
#pragma once
#include <vector>
#include <unordered_map>
#include <sys/epoll.h>
#include <iostream>
#include "Channel.h"
#include "../Utils/Timer.h"
// #include "Http.h"
class Http;

typedef std::shared_ptr<Channel> SharedChannel;
typedef std::shared_ptr<Http> SharedHttp;

class Epoll{

    public:
        Epoll();
        ~Epoll();
        void EpollAdd(SharedChannel channel, int timeout);
        void EpollModify(SharedChannel channel, int timeout);
        void EpollDel(SharedChannel channel);
        std::vector<SharedChannel> EpollWait();
        /* todo 调用会多次拷贝 */
        std::vector<epoll_event> GetReturnedFdEvents(){
            return returned_fd_events_;
        }
        void HandleExpire(){
            timer_manager_.HandleExpire();
        }
        // std::vector<int> to_listen_fds_;


    private:
        const int MAXFDS_ = 10000;
        /* 一个fd监听的最多的event数量 */
        const int MAXEVENTS = 4096;
        /* 5s */
        const int EPOLLWAIT_TIMEOUT = 5 * 1000;
        int epoll_fd_;
        // std::vector<epoll_event> returned_fd_events_;
        std::unordered_map<int,SharedChannel> fd2channel_;
        std::unordered_map<int,SharedHttp> fd2http_;

        std::vector<epoll_event> returned_fd_events_;

        TimerManager timer_manager_;

        void add_timer(SharedChannel shared_channel, int timeout);

        
};
