/*
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-09-01 19:29:58
 * @LastEditTime: 2022-10-03 13:02:53
 * @Description: 
 * Copyright (c) 2022 by Liyangfan.justin, All Rights Reserved. 
 */
#include "Utils.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>

const int MAX_BUF_SIZE = 4 * 1024;

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

    //解决bug7:Address already in use
    int flag = 1;
	if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) < 0)
	{
        perror("setsockopt failed");
		return -1;
	}

    struct sockaddr_in sever_addr;
    bzero((char *)&sever_addr,sizeof(sever_addr));
    sever_addr.sin_family = AF_INET;
    /* 转为一个整数形式 */
    sever_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    sever_addr.sin_port = htons((unsigned short)port);
    if(bind(listen_fd,(struct sockaddr *)&sever_addr,sizeof(sever_addr))==-1){
        perror("bind failed");
        close(listen_fd);
        return -1;
    }

    /* 开始监听客户端连接,成功返回0,错误返回-1 */
    if(listen(listen_fd,2048)==-1){
        perror("listen failed");
        close(listen_fd);
        return -1;
    }

    //无效监听描述符,客户端可以发起第一次握手
    if(listen_fd == -1){
        perror("invalid listen_fd");
        close(listen_fd);
        return -1;
    }

    return listen_fd;
}

void Utils::WriteToFd(int fd,void *buf,size_t size){
    char* ptr = (char *)buf;
    write(fd,ptr,size);
}

ssize_t  Utils::ReadFromFd(int fd,std::string& in_buffer){
    char buf[MAX_BUF_SIZE];
    ssize_t  read_size = 0;
    ssize_t  per_read_size = 0;
    while(true){
        // std::cout<<"readfromfd"<<std::endl;
        per_read_size = read(fd,buf,MAX_BUF_SIZE);
        if(per_read_size<0){
            if(errno == EAGAIN){
                return read_size;
            }
            else if(errno == EINTR){
                // break;
                continue;
            }else{
                perror("read error");
                return -1;
            }
        }else if(per_read_size==0){
            break;
        }else{
            read_size += per_read_size;
            in_buffer += std::string(buf, buf + per_read_size);
        }
    }
    return read_size;
}

ssize_t Utils::WriteToFd(int fd,std::string& out_buffer){
    ssize_t left = out_buffer.length();
    ssize_t writen_sum = 0;
    ssize_t writen = 0;
    /* const指针,指针所指向的为cosnt */
    const char* ptr = out_buffer.c_str();
    while(left>0){
        writen = write(fd,ptr,left);
        if(writen<0){
            if(errno == EINTR){
                continue;
            }else if(errno == EAGAIN){
                break;
            }else{
                return -1;
            }
        }else if(writen==0){
            break;
        }
        writen_sum += writen;
        ptr += writen;
        left -= writen;
    }
    out_buffer = out_buffer.substr(writen_sum);
    return writen_sum;
}

/* 设置文件非阻塞 */
int Utils::setSocketNonBlocking(int fd) {
  int flag = fcntl(fd, F_GETFL, 0);
  if (flag == -1) return -1;

  flag |= O_NONBLOCK;
  if (fcntl(fd, F_SETFL, flag) == -1) return -1;
  return 0;
}
