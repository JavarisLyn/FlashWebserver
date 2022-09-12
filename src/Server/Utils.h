/*
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-09-01 19:30:10
 * @LastEditTime: 2022-09-12 13:56:39
 * @Description: 
 * Copyright (c) 2022 by Liyangfan.justin, All Rights Reserved. 
 */
#pragma once
#include <sys/syscall.h>
#include <unistd.h>
namespace Utils{
    #define gettid() syscall(SYS_gettid)
    int SocketBindListen(int port);
    void WriteToFd(int fd,void *buf,size_t size);
}


