/*
 * @Descripttion: 
 * @version: 
 * @Author: justin
 * @Date: 2022-08-23 14:17:10
 * @LastEditTime: 2022-09-13 15:06:10
 * @copyright: Copyright (c) 2022
 */
#include<iostream>
#include "Server/Epoll.h"
#include "Server/Utils.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h> 
using namespace std;

int main(){
    cout<<"Hello FlashWebserver!"<<endl;

    int listen_fd = Utils::SocketBindListen(8081);
    struct sockaddr_in client_addr;
    bzero((char *)&client_addr,sizeof(client_addr));
    socklen_t client_addr_len = sizeof(client_addr);
    /* 注意accept_fd和listen_fd的区别 */
    int accept_fd = 0;
    while((accept_fd = accept(listen_fd,(struct sockaddr *)&client_addr,&client_addr_len))>0){
        //建立新连接
        /* s_addr的ip形式是整数形式 */
        unsigned char *pch = (unsigned char*)&client_addr.sin_addr.s_addr;
        char szIPAddr[64] = {0};
        sprintf(szIPAddr, "%d.%d.%d.%d", *pch, *(pch+1), *(pch+2),*(pch+3));
        cout<<"client addr: "<<szIPAddr<<endl;

        char recv_buf[512];
        /* #include <unistd.h>  */
        int len = read(accept_fd,recv_buf,sizeof(recv_buf));
        cout<<recv_buf<<endl;

        char send_buf[256];
        char sendbuf[256] = "hello this is the first msg from server by leeeyf";
        write(accept_fd,sendbuf,sizeof(sendbuf));

        memcpy(send_buf,send_buf,10);
    }
}