/*
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-09-05 11:07:40
 * @LastEditTime: 2022-10-09 20:46:39
 * @Description: 
 * Copyright (c) 2022 by Liyangfan.justin, All Rights Reserved. 
 */
#include "./Utils/Utils.h"
#include "Server/EventLoop.h"
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include "Server/EventLoopThreadPool.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include "Server/Server.h"
using namespace std;

void signalHandler( int signum ) 
{ 
    cout<<"end"<<endl;
    _exit(0);
 }

int main(){
    EventLoop* eventloop = new EventLoop();
    Server server(eventloop,8088,3);
    server.start();
    server.shutdown();
    delete eventloop;
}
