/*
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-09-01 16:56:44
 * @LastEditTime: 2022-10-03 12:37:17
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

/* 前向声明，因为Channel包含Eventloop，同时Eventloop也包含Channel，include会导致循环引用 */
/* https://www.jianshu.com/p/9e247c805d5a */
class EventLoop;
class Http;

class Channel{

private:

    typedef std::function<void()> Callback;

    /* 处理的fd */
    int fd_;

    /* 所属的EventLoop */
    EventLoop* eventloop_;

    /* 所属的应用层对象,用weak ptr防止循环引用 */
    std::weak_ptr<Http> holder_;

    /* 监听到的活跃的事件,是to_listen_events的子集 */
    __uint32_t active_events_;

    /* 需要监听的事件 */
    __uint32_t to_listen_events_;

    /* 对应事件的回调函数 */
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

    /* todo 这里比较复杂，牵扯到左值、右值、万能引用、函数模板 */
    void SetReadCallback(Callback read_callback);
    void SetWriteCallback(Callback write_callback);
    void SetErrorCallback(Callback error_callback);
    void SetConnCallback(Callback conn_callback);

    /* 获取和更新需要监听的事件 */
    __uint32_t GetToListenEvents();
    void SetToListenEvents(__uint32_t new_events);
    void SetActiveEvents(__uint32_t active_events);

public:
    /* 处理监听到的事件 */
    void HandleEvents();
    void HandleRead();
    void HandleWrite();
    void HandleError();
    void HandleConn();

    void SetActiveEvents();
    void SetHolder(std::shared_ptr<Http> holder){
        /* weak_ptr和shared_ptr可以相互转化，shared_ptr可以直接赋值给weak_ptr
        weak_ptr也可以通过调用lock函数来获得shared_ptr */
        holder_ = holder;
    }
    std::shared_ptr<Http> GetHolder(){
        /* 如果weak_ptr对象已过期，返回一个空shared_ptr对象；否则，
        返回一个weak_ptr内部指针相关联的shared_ptr对象
        因此，shared_ptr对象的引用计数+1 */
        std::shared_ptr<Http> ret(holder_.lock());
        return ret;
    }

};