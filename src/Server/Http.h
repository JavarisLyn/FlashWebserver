/*
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-09-20 14:34:41
 * @LastEditTime: 2022-10-03 15:17:47
 * @Description: 
 * Copyright (c) 2022 by Liyangfan.justin, All Rights Reserved. 
 */
#pragma once
#include "EventLoop.h"
#include "Channel.h"
#include "Utils.h"

enum HttpVersion{
    HTTP_10 = 1,
    HTTP_11
};

enum ParseURIState{
    
};

class Http : public std::enable_shared_from_this<Http>{
    public:
        Http(EventLoop* eventloop,int fd);
        ~Http();

    private:
        //所属于的eventloop
        EventLoop* eventloop_;
        //负责该http连接的channel
        std::shared_ptr<Channel> channel_;
        //该连接的fd
        int fd_;
        //读写缓冲区
        std::string read_buffer_;
        std::string write_buffer_;
        //http版本
        HttpVersion http_version_;
        //是否支持keepalive
        bool keep_alive_;


    private:
        void HandleRead();
        void HandleWrite();
        void HandleError();
        void HandleClose();
        ParseURIState ParseURI();
    
    public:
        SharedChannel GetChannel(){
            return channel_;
        }

        EventLoop* GetLoop(){
            return eventloop_;
        }

        void Init();
    


};