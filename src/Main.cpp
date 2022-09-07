/*
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-09-05 11:07:40
 * @LastEditTime: 2022-09-05 17:24:10
 * @Description: 
 * Copyright (c) 2022 by Liyangfan.justin, All Rights Reserved. 
 */
#include "Server/Utils.h"
#include "Server/EventLoop.h"
#include <iostream>
#include <signal.h>
#include <unistd.h>
using namespace std;

void signalHandler( int signum ) 
{ 
    cout<<"end"<<endl;
    _exit(0);
 }

int main(){
    
    EventLoop* eventloop = new EventLoop();
    int port = 8088;
    int fd = SocketBindListen(port);
    // signal(SIGINT, signalHandler);
    UniqChannle channel(new Channel(eventloop,fd));
    channel->SetConnCallback([](){
        std::cout<<"new conn"<<std::endl;
    });
    channel->SetReadCallback([](){
        std::cout<<"new read"<<std::endl;
    });
    channel->SetToListenEvents(EPOLLIN | EPOLLET);
    
    eventloop->AddToEpoller(std::move(channel));
    eventloop->Loop();

}