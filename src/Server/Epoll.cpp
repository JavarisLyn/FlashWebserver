/*
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-09-01 17:08:39
 * @LastEditTime: 2022-09-05 19:20:20
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
void Epoll::EpollAdd(UniqChannle channel){
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
    fd2channel_.insert(std::pair<int,UniqChannle>(fd,std::move(channel)));
}

void Epoll::EpollModify(UniqChannle channel){
    int fd = channel->Getfd();
    struct epoll_event *event;
    event->events = channel->GetToListenEvents();
    if(epoll_ctl(epoll_fd_,EPOLL_CTL_MOD,fd,event)<0){
        perror("epoll ctl modify error");
    }else{
        std::cout<<"epoll ctl modify success"<<std::endl;
    }
}

void Epoll::EpollDel(UniqChannle channel){
    int fd = channel->Getfd();
    struct epoll_event *event;
    event->events = channel->GetToListenEvents();
    if(epoll_ctl(epoll_fd_,EPOLL_CTL_DEL,fd,event)<0){
        perror("epoll ctl delete error");
    }else{
        std::cout<<"epoll ctl delete success"<<std::endl;
    }
    fd2channel_.erase(fd);
}

std::vector<UniqChannle> Epoll::EpollWait(){
    // epoll_event events_begin = *(GetReturnedFdEvents().begin());//
    while(true){
        int fd_count = epoll_wait(epoll_fd_,&*returned_fd_events_.begin(),MAXEVENTS,EPOLLWAIT_TIMEOUT);
        if(fd_count<0){
            perror("epoll wait error");
        }
        std::vector<UniqChannle> active_channels;
        std::cout<<"epoll wait returned"<<fd_count<<std::endl;
        for(int i=0;i<fd_count;i++){
            int fd = returned_fd_events_[i].data.fd;
            std::cout<<"fd "<<i<<":"<<fd<<std::endl;
            /* how ?需要加move */
            UniqChannle channel = std::move(fd2channel_[fd]);
            channel->SetActiveEvents(returned_fd_events_[i].events);
            /* todo 是否需要设置待监听事件为空? */
            // channel.SetToListenEvents(0);
            active_channels.emplace_back(std::move(channel));
        }
        /* 拷贝问题？ */
        if(active_channels.size()>0){
            return active_channels;
        }
    }
}

