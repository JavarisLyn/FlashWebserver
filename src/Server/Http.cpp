/*
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-09-20 14:34:54
 * @LastEditTime: 2022-10-03 15:16:51
 * @Description: 
 * Copyright (c) 2022 by Liyangfan.justin, All Rights Reserved. 
 */
#include "Http.h"

Http::Http(EventLoop* eventloop,int fd):
    eventloop_(eventloop),
    fd_(fd),
    channel_(new Channel(eventloop,fd)),
    http_version_(HTTP_11),
    keep_alive_(false){
        channel_->SetReadCallback(std::bind(&Http::HandleRead,this));
        std::cout<<"http created"<<std::endl;
        LOG_TRACE("http construct,fd:%d\n",fd_);
    }

Http::~Http(){
    close(fd_);
     LOG_TRACE("http deconstruct,fd:%d\n",fd_);
    std::cout<<"http deconstruct"<<std::endl;
}

void Http::HandleRead(){
    int read_size = Utils::ReadFromFd(fd_,read_buffer_);
    // std::cout<<"http data:"<<std::endl;
    // std::cout<<read_buffer_<<std::endl;
    HandleWrite();
    HandleClose();
}

void Http::HandleWrite(){
    write_buffer_ = "HTTP/1.1 200 OK\r\nContent-type: text/plain\r\n\r\nHello World";
    int write_size = Utils::WriteToFd(fd_,write_buffer_);
}

void Http::HandleClose(){
    eventloop_->RemoveFromEpoller(channel_);
    std::cout<<"handle close finished"<<std::endl;
}

void Http::Init(){
    channel_->SetHolder(shared_from_this());
    channel_->SetToListenEvents(EPOLLIN | EPOLLET);
    eventloop_->AddToEpoller(channel_);

}