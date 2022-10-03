/*
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-09-01 17:08:39
 * @LastEditTime: 2022-10-03 15:10:31
 * @Description: 
 * Copyright (c) 2022 by Liyangfan.justin, All Rights Reserved. 
 */
#include <assert.h>
#include <sys/epoll.h>
#include "Epoll.h"
#include <iostream>

/* epoll_create和epoll_create1的不同 */
Epoll::Epoll()
    :epoll_fd_(epoll_create1(EPOLL_CLOEXEC)),
    returned_fd_events_(MAXEVENTS)
{
    assert(epoll_fd_>0);
}

Epoll::~Epoll(){}

/* channel用哪种智能指针? */
void Epoll::EpollAdd(SharedChannel channel){
    int fd = channel->Getfd();
    struct epoll_event event;
    event.data.fd = fd;//这个必须加
    std::cout<<"add fd"<<fd<<std::endl;
    event.events = channel->GetToListenEvents();
    if(epoll_ctl(epoll_fd_,EPOLL_CTL_ADD,fd,&event)<0){
        perror("epoll ctl error");
    }else{
        std::cout<<"epoll add success"<<std::endl;
    }
    /* how ?不加move报错 */
    fd2channel_.insert(std::pair<int,SharedChannel>(fd,channel));
    fd2http_.insert(std::pair<int,std::shared_ptr<Http>>(fd,channel->GetHolder()));
    LOG_TRACE("fd2channel_ isnerrt,fd:%d",fd);
}

void Epoll::EpollModify(SharedChannel channel){
    int fd = channel->Getfd();
    struct epoll_event *event;
    event->events = channel->GetToListenEvents();
    if(epoll_ctl(epoll_fd_,EPOLL_CTL_MOD,fd,event)<0){
        perror("epoll ctl modify error");
    }else{
        std::cout<<"epoll ctl modify success"<<std::endl;
    }
}

void Epoll::EpollDel(SharedChannel channel){
    int fd = channel->Getfd();
    struct epoll_event *event;
    event->events = channel->GetToListenEvents();
    if(epoll_ctl(epoll_fd_,EPOLL_CTL_DEL,fd,event)<0){
        LOG_TRACE("epoll ctl delete error%d\n",fd);
        perror("epoll ctl delete error");
    }else{
        LOG_TRACE("epoll ctl delete success%d\n",fd);
        std::cout<<"epoll ctl delete success"<<std::endl;
    }
    size_t n = fd2channel_.erase(fd);
    (void)n;
    assert(n == 1);
    fd2http_.erase(fd);
}

std::vector<SharedChannel> Epoll::EpollWait(){
    // epoll_event events_begin = *(GetReturnedFdEvents().begin());//
    while(true){
        int fd_count = epoll_wait(epoll_fd_,&*returned_fd_events_.begin(),MAXEVENTS,EPOLLWAIT_TIMEOUT);
        if(fd_count<0){
            perror("epoll wait error");
        }
        std::vector<SharedChannel> active_channels;
        std::cout<<"epoll wait returned"<<fd_count<<std::endl;
        for(int i=0;i<fd_count;i++){
            int fd = returned_fd_events_[i].data.fd;
            std::cout<<"fd "<<i<<":"<<fd<<std::endl;
            /* how ?需要加move */
            SharedChannel channel = (fd2channel_[fd]);
            if(channel == nullptr){
                std::string channelFds = "";
                for(auto& t:fd2channel_){
                    channelFds += std::to_string(t.first);
                }
                LOG_TRACE("channel nullptr:%s,map:%d\n",channelFds,fd);
                continue;
            }
            channel->SetActiveEvents(returned_fd_events_[i].events);
            /* todo 是否需要设置待监听事件为空? */
            // channel.SetToListenEvents(0);
            active_channels.emplace_back((channel));
        }
        /* 拷贝问题？ */
        if(active_channels.size()>0){
            return active_channels;
        }
    }
}

