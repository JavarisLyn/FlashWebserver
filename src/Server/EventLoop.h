/*
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-09-01 16:59:45
 * @LastEditTime: 2022-09-30 17:44:51
 * @Description: 
 * Copyright (c) 2022 by Liyangfan.justin, All Rights Reserved. 
 */
#pragma once
#include <memory>
#include "Epoll.h"
// #include "Channel.h"
#include <mutex>

class EventLoop{
    public:

        typedef std::function<void()> Callback; 

        EventLoop();
        ~EventLoop();

        void Loop();
        void Quit();

        void AddToEpoller(SharedChannel channel){
            epoller_->EpollAdd(std::move(channel));
        }

        bool IsInLoopthread() const;
        void RunFunction(Callback cb);

        void RemoveFromEpoller(SharedChannel channel){
            epoller_->EpollDel(channel);
        }
        

    private:
        void WakeUpTargetEventLoop();
        void DoCallbacks();
        int CreateEventFd();
        /* &&? todo */
        /* 需要从RunFunction调用 */
        void QueueINCallback(Callback cb);

    private:
        /* 控制loop启停 */
        bool is_looping_;
        /* 无需使用shared_ptr */
        std::unique_ptr<Epoll> epoller_;

        /* 其他线程需要本线程执行的函数放在该队列里，所以要注意并发控制 */
        /* 1.其他线程添加回调时互斥 2.本线程执行和其他线程添加时互斥 */
        std::vector<Callback> callback_list_;

        std::mutex callback_list_mutex_;

        /* 是否正在执行队列里的回调函数 */
        bool running_callbacks_;

        /* 该eventloop所属的线程id */
        const int thread_id_;

        /* 专门用于唤醒epoll的fd和channel */
        int wakeup_fd_;

        SharedChannel wakeup_channel_;
           
};