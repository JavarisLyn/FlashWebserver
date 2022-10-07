/*
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-09-20 14:34:41
 * @LastEditTime: 2022-10-07 12:31:07
 * @Description: 
 * Copyright (c) 2022 by Liyangfan.justin, All Rights Reserved. 
 */
#pragma once
// #include "EventLoop.h"
#include "Channel.h"
#include "Utils.h"
#include "../Utils/Timer.h"
#include <unordered_map>
class EventLoop;
class TimerNode;

enum HttpVersion{
    HTTP_10 = 1,
    HTTP_11
};

enum ProcessState{
    STATE_PARSE_URI = 1,//处理uri
    STATE_PARSE_HEADER,//处理头部
    STATE_PARSE_BODY,//处理body内容,只有post才有
    STATE_PROCESS,//分析处理请求
    STATE_FINISH//请求处理完成
};

enum ParseURIState{
    PARSE_URI_AGAIN = 1,//解析未完成,继续读取后再重新解析
    PARSE_URI_ERROR,//解析失败
    PARSE_URI_SUCCESS,//解析成功
};

enum ParseHeaderState{
    PARSE_HEADER_AGAIN = 1,
    PARSE_HEADER_SUCCESS,
    PARSE_HEADER_ERROR
};

enum HeaderLineState{
    H_START = 1,
    H_KEY,//key
    H_COLON,//:
    H_SPACES_AFTER_COLON,// 
    H_VALUE,//value
    H_CR,//\r
    H_LF,//\n
    H_END_CR,//\r
    H_END_LF//\n
};

//TODO post method implementation
enum ParseBodyState{

};

enum ProcessRequestState{
    ProcessSuccess = 1,
    ProcessError
};

enum ConnectionState{
    CONNECTED = 1,
    DISCONNECTING,//shutdown只关闭了读或写端
    DISCONNECTED
};

enum HttpMethod{
    METHOD_GET = 1,
    METHOD_POST,
    //TODO HEAD method
    //METHOD_HEAD
};


class Http : public std::enable_shared_from_this<Http>{
    public:
        Http(EventLoop* eventloop,int fd,bool keep_alive);
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
        //请求方法
        HttpMethod method_;
        //请求file path
        std::string file_path_;
        //http版本
        HttpVersion http_version_;
        //header key-value
        std::unordered_map<std::string,std::string> header_kv_;
        //parse headers state
        HeaderLineState header_line_state;
        //是否支持keepalive
        bool keep_alive_;

        ProcessState process_state_;

        ConnectionState connectionState_;

        bool has_error_;

        // static pthread_once_t once_control;

        //timer
        std::weak_ptr<TimerNode> timer_;

    private:
        void HandleRead();
        void HandleWrite();
        void HandleError();
        void HandleCurrentConn();
        ParseURIState ParseURI();
        ParseHeaderState ParseHeaders();
        ProcessRequestState ProcessRequest();
        void HandleError(int fd, int err_code, std::string err_msg);
        void reset();
        static void InitResourceMap();
        std::string GetResourceType(const std::string& suffix);
        void seperateTimer();
    public:
        void HandleClose();

        std::shared_ptr<Channel> GetChannel(){
            return channel_;
        }

        EventLoop* GetLoop(){
            return eventloop_;
        }

        void Init();

        void LinkTimer(std::shared_ptr<TimerNode> timer){
            timer_ = timer;
        }
    


};