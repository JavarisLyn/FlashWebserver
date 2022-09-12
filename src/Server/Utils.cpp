/*
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-09-01 19:29:58
 * @LastEditTime: 2022-09-12 13:57:44
 * @Description: 
 * Copyright (c) 2022 by Liyangfan.justin, All Rights Reserved. 
 */
#include "Utils.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

int Utils::SocketBindListen(int port){
    if(port<0 || port>65535){
        return -1;
    }

    int listen_fd = 0;
    if((listen_fd = socket(AF_INET, SOCK_STREAM,0))==-1){
        close(listen_fd);
        std::cout<<"socket creat failed"<<std::endl;
        return -1;
    }

    struct sockaddr_in sever_addr;
    bzero((char *)&sever_addr,sizeof(sever_addr));
    sever_addr.sin_family = AF_INET;
    /* 转为一个整数形式 */
    sever_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    sever_addr.sin_port = htons((unsigned short)port);
    if(bind(listen_fd,(struct sockaddr *)&sever_addr,sizeof(sever_addr))==-1){
        close(listen_fd);
        return -1;
    }

    /* 开始监听客户端连接,成功返回0,错误返回-1 */
    if(listen(listen_fd,2048)==-1){
        close(listen_fd);
        return -1;
    }

    //无效监听描述符,客户端可以发起第一次握手
    if(listen_fd == -1){
        close(listen_fd);
        return -1;
    }

    return listen_fd;
}

void Utils::WriteToFd(int fd,void *buf,size_t size){
    char* ptr = (char *)buf;
    write(fd,ptr,size);
}

