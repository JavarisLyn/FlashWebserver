/*
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-09-20 14:34:54
 * @LastEditTime: 2022-10-07 12:33:47
 * @Description: 
 * Copyright (c) 2022 by Liyangfan.justin, All Rights Reserved. 
 */
#include "Http.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "EventLoop.h"

const int DEFAULT_EXPIRE_TIME = 3 * 1000;//3s expire time if keep-alive = flase
const int DEFAULT_KEEP_ALIVE_TIME = 60 * 1000;


//资源类型
std::unordered_map<std::string,std::string> resource_type_map_;
pthread_once_t once_control = PTHREAD_ONCE_INIT;
Http::Http(EventLoop* eventloop,int fd,bool keep_alive):
    eventloop_(eventloop),
    fd_(fd),
    channel_(new Channel(eventloop,fd)),
    http_version_(HTTP_11),
    header_line_state(H_START),
    process_state_(STATE_PARSE_URI),
    connectionState_(CONNECTED),
    has_error_(false),
    keep_alive_(keep_alive){
        channel_->SetReadCallback(std::bind(&Http::HandleRead,this));
        channel_->SetWriteCallback(std::bind(&Http::HandleWrite,this));
        channel_->SetConnCallback(std::bind(&Http::HandleCurrentConn,this));
        //std::cout<<"http created"<<std::endl;
        LOG_TRACE("http construct,fd:%d\n",fd_);
    }

Http::~Http(){
    close(fd_);
    LOG_TRACE("http deconstruct,fd:%d\n",fd_);
    //std::cout<<"http deconstruct,connection closed"<<std::endl;
}

void Http::InitResourceMap(){
  resource_type_map_[".html"] = "text/html";
  resource_type_map_[".avi"] = "video/x-msvideo";
  resource_type_map_[".bmp"] = "image/bmp";
  resource_type_map_[".c"] = "text/plain";
  resource_type_map_[".doc"] = "application/msword";
  resource_type_map_[".gif"] = "image/gif";
  resource_type_map_[".gz"] = "application/x-gzip";
  resource_type_map_[".htm"] = "text/html";
  resource_type_map_[".ico"] = "image/x-icon";
  resource_type_map_[".jpg"] = "image/jpeg";
  resource_type_map_[".png"] = "image/png";
  resource_type_map_[".txt"] = "text/plain";
  resource_type_map_[".mp3"] = "audio/mp3";
  resource_type_map_["default"] = "text/html";
}

std::string Http::GetResourceType(const std::string& suffix){
  // std::function<void()> func = std::bind(&Http::InitResourceMap,this);
  // pthread_once(&once_control,static_cast<void(*)()>(&func));
  pthread_once(&once_control,InitResourceMap);
  if(resource_type_map_.find(suffix) == resource_type_map_.end()){
    return resource_type_map_["default"];
  }else{
    return resource_type_map_[suffix];
  }
  
}

void Http::HandleRead(){
    //<<"handle http read"<<std::endl;
    /* 这里采用追加写inbuffer的方式,一次请求的数据可能需要多次写入才能完成 */
    __uint32_t events = channel_->GetToListenEvents();
    do{
      bool zero = false;
      int read_size = Utils::ReadFromFd(fd_,read_buffer_,zero);
      if (connectionState_ == DISCONNECTING) {
        read_buffer_.clear();
        break;
      }
      //std::cout<<"readsize"<<read_size<<"zero"<<zero<<std::endl;
      if(read_size<0){
        has_error_ = true;
        perror("read buffer size < 0");
        HandleError(fd_,400,"Bad Request");
        break;
      }else if(zero == true){
        //没有读到数据,两个原因
        //1.对端已经关闭了连接，这时再写该fd会出错(received signal SIGPIPE, Broken pipe)，此时应该关闭连接
        //2.对端只是shutdown()了写端，告诉server我已经写完了，但是还可以接收信息。server应该在写完所有的信息后再关闭连接。
        //更优雅的做法是透明的传递这个行为，即server顺着关闭读端，然后发完数据后关闭。
        connectionState_ = DISCONNECTING;
        if(read_size == 0){
          // has_error_ = true;
          break;
        }
      }
      if(process_state_ == STATE_PARSE_URI){
        ParseURIState uri_process_state = ParseURI();
        if(uri_process_state == PARSE_URI_AGAIN){
          break;
        }else if(uri_process_state == PARSE_URI_ERROR){
          has_error_ = true;
          perror("uri process error");
          HandleError(fd_,400,"Bad Request");
          break;
        }else{
          process_state_ = STATE_PARSE_HEADER;
        }
      }

      if(process_state_ == STATE_PARSE_HEADER){
        ParseHeaderState header_parse_state = ParseHeaders();
        if(header_parse_state == PARSE_HEADER_AGAIN){
          break;
        }else if(header_parse_state == PARSE_HEADER_ERROR){
          has_error_ = true;
          perror("header process error");
          HandleError(fd_,400,"Bad Request");
          break;
        }else{
          if(method_ == METHOD_POST){
            process_state_ = STATE_PARSE_BODY;
          }else if(method_ == METHOD_GET){
            process_state_ = STATE_PROCESS;
          }
        }
      }

      if(process_state_ == STATE_PARSE_BODY){

      }

      if(process_state_ == STATE_PROCESS){
        ProcessRequestState request_process_state = ProcessRequest();
        if(request_process_state == ProcessError){
          has_error_ = true;
          perror("request process error");
          HandleError(fd_,400,"Bad Request");
          break;
        }else if(request_process_state == ProcessSuccess){
          process_state_ = STATE_FINISH;
        }
      }
    }while(false);
    
    if(!has_error_){
      if(write_buffer_.size() > 0){
        HandleWrite();
      }

      if(!has_error_ && process_state_ == STATE_FINISH){
        //1.HandleClose();直接关闭
        //2.支持管线化,缓冲区中可能有多个http数据报
        reset();
        if(read_buffer_.size() > 0 && connectionState_ != DISCONNECTING){
          HandleRead();
        }
      }else if(!has_error_ && connectionState_ != DISCONNECTED){
        //还没处理完 比如AGAIN状态
        events |= EPOLLIN;
        channel_->SetToListenEvents(events);
        //HandleRead();
      }
    }else{
      // HandleError(fd_,400,"Bad Request");
      HandleClose();
      //std::cout<<"handle close"<<std::endl;
    }


    // std::cout<<"http data:"<<std::endl;
    // std::cout<<read_buffer_<<std::endl;
    // HandleClose();
}

void Http::HandleCurrentConn(){
  //std::cout<<"handle currentConn"<<std::endl;
  seperateTimer();
  __uint32_t events = channel_->GetToListenEvents();
  if(!has_error_ && connectionState_ == CONNECTED){
    if(events != 0){
      if ((events & EPOLLIN) && (events & EPOLLOUT)) {
        events = __uint32_t(0);
        events |= EPOLLOUT;
      }
      events |= EPOLLET;
      channel_->SetToListenEvents(EPOLLOUT | EPOLLET);
      if(keep_alive_){
        //std::cout<<"1"<<std::endl;
        eventloop_->UpdateEpoller(channel_,DEFAULT_KEEP_ALIVE_TIME);
      }else{
        //std::cout<<"2"<<std::endl;
        eventloop_->UpdateEpoller(channel_,DEFAULT_EXPIRE_TIME);
      }
    }else{
      if(keep_alive_){
        //std::cout<<"3"<<std::endl;
        events |= (EPOLLIN | EPOLLET);
        channel_->SetToListenEvents(events);
        eventloop_->UpdateEpoller(channel_,DEFAULT_KEEP_ALIVE_TIME);
      }else{
        //<<"4"<<std::endl;
        // events |= (EPOLLIN | EPOLLET);
        // channel_->SetToListenEvents(events);
        // eventloop_->UpdateEpoller(channel_,DEFAULT_EXPIRE_TIME);
        //veents = 0 means no more IO needed,close directly if short conn
        HandleClose();
      }
    }
  }else if(!has_error_ && connectionState_ == DISCONNECTING){
    if(events & EPOLLOUT){
      channel_->SetToListenEvents(EPOLLOUT | EPOLLET);
    }else{
      HandleClose();
    }
  }
}

ParseURIState Http::ParseURI(){
    int pos = read_buffer_.find('/r');
    if(pos<0){
        return PARSE_URI_AGAIN;
    }

    //剩下的只在header_line操作
    std::string header_line = read_buffer_.substr(0,pos);
    if(read_buffer_.size() > pos + 1){
        //这里考虑后面优化成用下标记录当前处理到的位置,免去string copy
        read_buffer_ = read_buffer_.substr(pos+1);
    }else{
        read_buffer_.clear();
    }
    

    //method
    int posGet = header_line.find("GET");
    int posPost = header_line.find("POST");
    //int posHead = read_buffer_.find("HEAD");

    if(posGet>=0){
        pos = posGet;
        method_ = METHOD_GET;
    }else if(posPost>=0){
        pos = posPost;
        method_ = METHOD_POST;
    }else{
        return PARSE_URI_ERROR;
    }

    //url
    // GET /search/users?q=JakeWharton HTTP/1.1
    pos = header_line.find("/",pos);
    if(pos<0){
        file_path_ = "index.html";
        http_version_ = HTTP_11;
        return PARSE_URI_SUCCESS;
    }else{
        int space_pos = header_line.find(" ",pos);
        if(space_pos<0){
            return PARSE_URI_ERROR;
        }else{
            if(space_pos - pos > 1){
                file_path_ = header_line.substr(pos+1,space_pos - pos -1);
                int question_mark_pos = file_path_.find('?');
                if(question_mark_pos>=0){
                    file_path_ = file_path_.substr(0,question_mark_pos);
                }
            }else{
                file_path_ = "index.html";
            }
        }
        pos = space_pos;
    }

    //http version
    pos = header_line.find("/",pos);
    if(pos<0){
        return PARSE_URI_ERROR;
    }else{
        if(header_line.size() - pos <= 3){
            return PARSE_URI_ERROR;
        }else{
            std::string version = header_line.substr(pos+1,3);
            if(version == "1.0"){
                http_version_ = HTTP_10;
            }else if(version == "1.1"){
                http_version_ = HTTP_11;
            }else{
                return PARSE_URI_ERROR;
            }
        }
    }
    
    return PARSE_URI_SUCCESS;
}

ParseHeaderState Http::ParseHeaders(){
  std::string &str = read_buffer_;
  int key_start = -1, key_end = -1, value_start = -1, value_end = -1;
  int now_read_line_begin = 0;
  bool notFinish = true;
  size_t i = 0;
  for (; i < str.size() && notFinish; ++i) {
    switch (header_line_state) {
      case H_START: {
        if (str[i] == '\n' || str[i] == '\r') break;
        header_line_state = H_KEY;
        key_start = i;
        now_read_line_begin = i;
        break;
      }
      case H_KEY: {
        if (str[i] == ':') {
          key_end = i;
          if (key_end - key_start <= 0) return PARSE_HEADER_ERROR;
          header_line_state = H_COLON;
        } else if (str[i] == '\n' || str[i] == '\r')
          return PARSE_HEADER_ERROR;
        break;
      }
      case H_COLON: {
        if (str[i] == ' ') {
          header_line_state = H_SPACES_AFTER_COLON;
        } else
          return PARSE_HEADER_ERROR;
        break;
      }
      case H_SPACES_AFTER_COLON: {
        header_line_state = H_VALUE;
        value_start = i;
        break;
      }
      case H_VALUE: {
        if (str[i] == '\r') {
          header_line_state = H_CR;
          value_end = i;
          if (value_end - value_start <= 0) return PARSE_HEADER_ERROR;
        } else if (i - value_start > 255)
          return PARSE_HEADER_ERROR;
        break;
      }
      case H_CR: {
        if (str[i] == '\n') {
          header_line_state = H_LF;
          std::string key(str.begin() + key_start, str.begin() + key_end);
          std::string value(str.begin() + value_start, str.begin() + value_end);
          header_kv_[key] = value;
          now_read_line_begin = i;
        } else
          return PARSE_HEADER_ERROR;
        break;
      }
      case H_LF: {
        if (str[i] == '\r') {
          header_line_state = H_END_CR;
        } else {
          key_start = i;
          header_line_state = H_KEY;
        }
        break;
      }
      case H_END_CR: {
        if (str[i] == '\n') {
          header_line_state = H_END_LF;
        } else
          return PARSE_HEADER_ERROR;
        break;
      }
      case H_END_LF: {
        notFinish = false;
        key_start = i;
        now_read_line_begin = i;
        break;
      }
    }
  }
  if (header_line_state == H_END_LF) {
    str = str.substr(i);
    return PARSE_HEADER_SUCCESS;
  }
  str = str.substr(now_read_line_begin);
  return PARSE_HEADER_AGAIN;
}

ProcessRequestState Http::ProcessRequest(){
    if(method_ == METHOD_POST){
        //TODO
    }else{
        std::string state_line = "HTTP/";
        if(http_version_ == HTTP_11){
            state_line += "1.1";
        }else if(http_version_ == HTTP_10){
            state_line += "1.0";
        }
        state_line += " 200 OK\r\n";
        write_buffer_ += state_line;

        std::string header;
        if (header_kv_.find("Connection") != header_kv_.end() &&
        (header_kv_["Connection"] == "Keep-Alive" || header_kv_["Connection"] == "keep-alive")) {
            keep_alive_ = true;
            header += std::string("Connection: Keep-Alive\r\n") + "Keep-Alive: timeout=" +
            std::to_string(DEFAULT_KEEP_ALIVE_TIME) + "\r\n";
        }
        
        //获取文件状态信息 stat.h
        struct stat sbuf;
        if (stat(file_path_.c_str(), &sbuf) < 0) {
          header.clear();
          HandleError(fd_, 404, "Not Found!");
          return ProcessError;
        }

        //filetype
        int dot_pos = file_path_.find('.');
        std::string filetype;
        if (dot_pos < 0)
          filetype = GetResourceType("default");
        else
          filetype = GetResourceType(file_path_.substr(dot_pos));

        header += "Content-Type: " + filetype + "\r\n";
        header += "Content-Length: " + std::to_string(sbuf.st_size) + "\r\n";
        header += "Server: FlashWeb Server\r\n";
        header += "\r\n";
        write_buffer_ += header;

        //fcntl.h
        int file_fd = open(file_path_.c_str(),O_RDONLY,0);
        if (file_fd < 0) {
          write_buffer_.clear();
          HandleError(fd_, 404, "Not Found!");
          return ProcessError;
        }
        void *mmapRet = mmap(NULL, sbuf.st_size, PROT_READ, MAP_PRIVATE, file_fd, 0);
        close(file_fd);
        if (mmapRet == (void *)-1) {
          munmap(mmapRet, sbuf.st_size);
          write_buffer_.clear();
          HandleError(fd_, 404, "Not Found!");
          return ProcessError;
        }
        char *src_addr = static_cast<char *>(mmapRet);
        write_buffer_ += std::string(src_addr, src_addr + sbuf.st_size);
        munmap(mmapRet, sbuf.st_size);
        return ProcessSuccess;

    }
}

void Http::HandleError(int fd, int err_code, std::string err_msg){
  err_msg = " " + err_msg;
  char send_buff[4096];
  std::string body_buff, header_buff;
  body_buff += "<html><title>哎~出错了</title>";
  body_buff += "<body bgcolor=\"ffffff\">";
  body_buff += std::to_string(err_code) + err_msg;
  body_buff += "<hr><em> Flash Web Server</em>\n</body></html>";

  header_buff += "HTTP/1.1 " + std::to_string(err_code) + err_msg + "\r\n";
  header_buff += "Content-Type: text/html\r\n";
  header_buff += "Connection: Close\r\n";
  header_buff += "Content-Length: " + std::to_string(body_buff.size()) + "\r\n";
  header_buff += "Server: Flash Web Server\r\n";
  ;
  header_buff += "\r\n";
  // 错误处理不考虑writen不完的情况
  sprintf(send_buff, "%s", header_buff.c_str());
  if(Utils::WriteToFd(fd, send_buff, strlen(send_buff))<0){
    perror("WriteToFd error");
  }
  sprintf(send_buff, "%s", body_buff.c_str());
  if(Utils::WriteToFd(fd, send_buff, strlen(send_buff))<0){
    perror("WriteToFd error");
  }
}

void Http::HandleWrite(){
  //std::cout<<"handle http write"<<std::endl;
    //write_buffer_ = "HTTP/1.1 200 OK\r\nContent-type: text/plain\r\n\r\nHello World";
    if(!has_error_ && connectionState_ != DISCONNECTED){
      __uint32_t events = channel_->GetToListenEvents();
      int write_size = Utils::WriteToFd(fd_,write_buffer_);
      if(Utils::WriteToFd(fd_, write_buffer_)<0){
        perror("write to buffer error");
      }
      if(write_buffer_.size() > 0){
        events |= EPOLLOUT;
        channel_->SetToListenEvents(events);
      }
    }
}

//TODO
void Http::seperateTimer(){
  //std::cout<<"seperate timer"<<std::endl;
  if(timer_.lock()){
    std::shared_ptr<TimerNode> share_timer(timer_);
    share_timer->ClearNode();//then deconstruct won't call handleClose.
    timer_.reset();
  }
}

void Http::reset(){
  file_path_.clear();
  process_state_ = STATE_PARSE_URI;
  header_line_state = H_START;
  header_kv_.clear();
  seperateTimer();
}

void Http::HandleClose(){
  //std::cout<<"handle close"<<std::endl;
  connectionState_ = DISCONNECTED;
  eventloop_->RemoveFromEpoller(channel_);
  // std::cout<<"handle close finished"<<std::endl;
}

void Http::Init(){
    channel_->SetHolder(shared_from_this());
    channel_->SetToListenEvents(EPOLLIN | EPOLLET);
    eventloop_->AddToEpoller(channel_,DEFAULT_EXPIRE_TIME);

}