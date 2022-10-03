/*
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-09-07 21:30:58
 * @LastEditTime: 2022-10-03 15:42:49
 * @Description: 
 * Copyright (c) 2022 by Liyangfan.justin, All Rights Reserved. 
 */
#include "Server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <functional>
#include <cstdio>

Server::Server(EventLoop * event_loop, int port,int thread_num):
    port_(port),
    thread_num_(thread_num),
    event_loop_(event_loop),
    eventLoopThreadPool_(new EventLoopThreadPool(5))  
{}

std::unique_ptr<FlashLogger::AsynLogger> asynLogger;

void output(const char* data, size_t len){
    asynLogger->append(data,len);
}


void Server::start(){
    FlashLogger::LogConfig logConfig;
    logConfig.flushInterval = 10;
    logConfig.initBufferNodeListSize = 3;
    logConfig.rollFileSize = 256 * 1024 * 1024;
    FlashLogger::Logger::setConfig(logConfig);
 
    FlashLogger::Logger::setOutPutFunc(output);
    asynLogger = std::unique_ptr<FlashLogger::AsynLogger>(new FlashLogger::AsynLogger());
    asynLogger->start();
    LOG_TRACE("server start","123");
    
    eventLoopThreadPool_->start();

    listen_fd_ = Utils::SocketBindListen(port_);
    std::cout<<"listen fd"<<listen_fd_<<std::endl;
    SharedChannel channel(new Channel(event_loop_,listen_fd_));
    channel->SetConnCallback(std::bind(&Server::HandleNewConn,this));
    channel->SetReadCallback([](){
        std::cout<<"new read"<<std::endl;
    });
    channel->SetToListenEvents(EPOLLIN | EPOLLET);
    
    event_loop_->AddToEpoller(std::move(channel));
    event_loop_->Loop();
}

void Server::HandleNewConn(){
    struct sockaddr_in client_addr;
    bzero((char *)&client_addr,sizeof(client_addr));
    socklen_t client_addr_len = sizeof(client_addr);
    /* 注意accept_fd和listen_fd的区别 */
    int accept_fd = 0;
    while((accept_fd = accept(listen_fd_,(struct sockaddr *)&client_addr,&client_addr_len))>0){
        //建立新连接
        std::cout<<"new conn,acept fd:"<<accept_fd<<std::endl;
        EventLoop *new_loop = eventLoopThreadPool_->GetNextLoop();
        SharedChannel channel(new Channel(new_loop,accept_fd));
        channel->SetReadCallback([](){
            std::cout<<"new read"<<std::endl;
        });
        channel->SetToListenEvents(EPOLLIN | EPOLLET);
        new_loop->RunFunction(std::bind(&EventLoop::AddToEpoller,new_loop,channel));
        // new_loop->AddToEpoller(std::move(channel));
    }
}