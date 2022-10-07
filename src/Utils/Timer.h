/*
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-10-06 10:08:45
 * @LastEditTime: 2022-10-06 16:27:18
 * @Description: 
 * Copyright (c) 2022 by Liyangfan.justin, All Rights Reserved. 
 */
#pragma once
#include <unistd.h>
#include <memory>
#include <queue>
#include <deque>
#include <sys/time.h>
#include "../Server/Http.h"

class TimerNode{
    public:
        TimerNode(std::shared_ptr<Http> data, int timeout);
        ~TimerNode();
        //update a timernode's expire time,timeout/ms
        void Update(int timeout);
        //check if a timernode is expired
        bool IsExpired();
        //clear this node
        void ClearNode();
        void SetDeleted(){is_deleted_ = true;}
        bool IsDeleted(){return is_deleted_;}
        std::size_t GetExpireTime(){return expire_time_;}

    private:
        bool is_deleted_;
        //ms
        size_t expire_time_;
        std::shared_ptr<Http> data_;
};

struct TimerCmp {
    bool operator()(std::shared_ptr<TimerNode> &a,
                    std::shared_ptr<TimerNode> &b) const {
    return a->GetExpireTime() > b->GetExpireTime();
    }
};

class TimerManager{
    public:
        // TimerManager();
        // ~TimerManager();
        void AddTimerNode(std::shared_ptr<Http>,int timeout);
        void HandleExpire();
    private:
        std::priority_queue<std::shared_ptr<TimerNode>,std::deque<std::shared_ptr<TimerNode>>,TimerCmp> timer_node_queue_; 
};

