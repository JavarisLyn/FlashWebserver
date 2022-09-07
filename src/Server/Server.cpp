/*
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-09-05 11:00:30
 * @LastEditTime: 2022-09-05 11:07:01
 * @Description: 
 * Copyright (c) 2022 by Liyangfan.justin, All Rights Reserved. 
 */
#include "Utils.h"
#include "EventLoop.h"
#include <iostream>
int main(){
    EventLoop* eventloop;
    int port = 8081;
    int fd = SocketBindListen(port);
    UniqChannle channel(new Channel(eventloop,fd));
    channel->SetConnCallback([](){
        std::cout<<"new conn"<<std::endl;
    });

    eventloop->AddToEpoller(std::move(channel));
    eventloop->Loop();

}