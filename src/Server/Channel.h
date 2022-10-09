/*
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-09-01 16:56:44
 * @LastEditTime: 2022-10-09 22:20:13
 * @Description: 一个Channel负责处理一个fd,属于一个EventLoop
 * Copyright (c) 2022 by Liyangfan.justin, All Rights Reserved. 
 */
#pragma once
#include <functional>
#include <sys/epoll.h>
#include <memory>
#include <lyf/Logger/AsynLogger.h>
#include <lyf/Logger/Logger.h>
#include <lyf/Logger/LogConfig.h>

/* forward declaration,to void circular reference(channel & eventloop) */
/* https://www.jianshu.com/p/9e247c805d5a */
class EventLoop;
class Http;

class Channel{

private:

    typedef std::function<void()> Callback;

    /* fd this channel to process */
    int fd_;

    /* eventLoop this channel belongs to */
    EventLoop* eventloop_;

    /* application layer obeject this channel belongs to,use weak ptr to avoid circular reference */
    std::weak_ptr<Http> holder_;

    /* active events returned from epoller, is subset of to_listen_events */
    __uint32_t active_events_;

    /* events need to be listend */
    __uint32_t to_listen_events_;

    /* callbacks */
    Callback read_callback_;
    Callback write_callback_;
    Callback error_callback_;
    Callback conn_callback_;
    
public:

    Channel() = default;
    Channel(EventLoop* eventLoop,int fd);
    ~Channel();

    int Getfd();
    void Setfd(int fd);

    void SetReadCallback(Callback&& read_callback);
    void SetWriteCallback(Callback&& write_callback);
    void SetErrorCallback(Callback&& error_callback);
    void SetConnCallback(Callback&& conn_callback);

    __uint32_t GetToListenEvents();
    void SetToListenEvents(__uint32_t new_events);
    void SetActiveEvents(__uint32_t active_events);

public:

    void HandleEvents();
    void HandleRead();
    void HandleWrite();
    void HandleError();
    void HandleConn();

    void SetActiveEvents();
    void SetHolder(std::shared_ptr<Http> holder){
        /* weak_ptr and shared_ptr can be transformed to each other, 
        a shared_ptr can be assigned to a weak_ptr directly,
        weak_ptr can be transformed to shared_ptr by weak_ptr.lock() */
        holder_ = holder;
    }
    std::shared_ptr<Http> GetHolder(){
        /* lock() return null shared_ptr if expired() == true(managed object is deleted),
        return shared_ptr if expired() == false,and use_count()++ */
        std::shared_ptr<Http> ret(holder_.lock());
        return ret;
    }

};