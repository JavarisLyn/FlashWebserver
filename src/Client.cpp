/*
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-09-01 20:55:16
 * @LastEditTime: 2022-09-01 22:18:49
 * @Description: 
 * Copyright (c) 2022 by Liyangfan.justin, All Rights Reserved. 
 */
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>

using namespace std;

/* 也可以直接用linux的nc命令 */

int main(){
    int sock = socket(AF_INET,SOCK_STREAM,0);

    struct sockaddr_in serv_addr;//首先要指定一个服务端的ip地址+端口，表明是向哪个服务端发起请求
    //先取地址再转换为指针
    bzero((char *)&serv_addr,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//注意，这里是服务端的ip和端口
    serv_addr.sin_port = htons(8081);
    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    //发送消息
    char sendbuf[256] = "hello this is the first msg from client by leeeyf";
    write(sock,sendbuf,sizeof(sendbuf));

    //接受消息
    char recvbuf[256];
    int len = read(sock,recvbuf,sizeof(recvbuf));
    cout<<recvbuf<<endl;

    //头文件 unistd.h
    close(sock);
    return 0;



}