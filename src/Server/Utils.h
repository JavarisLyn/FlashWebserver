/*
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-09-01 19:30:10
 * @LastEditTime: 2022-10-04 21:01:17
 * @Description: 
 * Copyright (c) 2022 by Liyangfan.justin, All Rights Reserved. 
 */
#pragma once
#include <sys/syscall.h>
#include <unistd.h>
#include <string>
namespace Utils{
    #define gettid() syscall(SYS_gettid)
    int SocketBindListen(int port);
    ssize_t WriteToFd(int fd,void *buf,size_t size);
    ssize_t ReadFromFd(int fd,std::string& in_buffer,bool& zero);
    ssize_t WriteToFd(int fd,std::string& out_buffer);
    int setSocketNonBlocking(int fd);
}


