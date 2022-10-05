# Description
### FlashWebserver is a C++ 11 based Webserver. On the basis of implementing basic tcp communication, HTTP service is supported, including most features of http1.0 & http1.1. 
# Environment
- 4vCPUs | 8GiB | c7.xlarge.2 CentOS 8.2 64bit | SSD 40 GB
- g++ (GCC) 8.5.0
- cmake version 3.3.2
- C++11
# Features
- Lightweight and high-performence
- Epoll & non-block socket
- Reactor
- Mutilthread
- Http,persistent connections and pipelining support
- Uses Modern C++
# Get Started
```
cd FlashWebserver/build/
sh ../build.sh
./FalshWebserver
```
# Benchmark
### webbench -c 500 -t 60 http://127.0.0.1:8088/index.html
```
[root@ecs-7073 ~]# webbench -c 500 -t 10 http://127.0.0.1:8088/index.html
Webbench - Simple Web Benchmark 1.5
Copyright (c) Radim Kolar 1997-2004, GPL Open Source Software.

Benchmarking: GET http://127.0.0.1:8088/index.html
500 clients, running 10 sec.

Speed=2150262 pages/min, 3798796 bytes/sec.
Requests: 358377 susceed, 0 failed.
```
# Framework
## Reactor
![http header parse](./pics/reactor.jpg)
### Main Objects
- Thread
- Eventloop
- Epoller
- Channel  

One eventloop per thread.\
One epoller per eventloop. Uses edge-triggered mode to monitoring multiple fds in one eventloop to see if I/O is possible on any ofthem.\
One eventloop contains multiple channels.\
One channel is responsible for a fd(socket connection)'s read,write,connection state update and close.  
  
The main thread which contains the main eventloop is responsible for the establishing of all new connections.then the round robin strategy is used to assign these connections to other eventloops(threads).Essentially,the fd(file descriptor) is assigned to the epoller of the target eventloop by EPOLL_CTL.

### Why is reactor faster
- mutil-thread,take full advantages of cpu
- non-block IO & epoll(edge-triggered), avoid connection waiting for IO one by one.


## HTTP
### HTTP URI Parsing
![http header parse](./pics/http_header_parse.jpg)
### pipelining
Before the response arrives, multiple requests can be put into the buffer. When the first request is sent to the server, the second and third requests can also be sent. It is unnecessary to wait until the first request responds back. Under high latency network conditions, this can reduce the network loopback time and improve performance.
### HTTP header parsing
### HTTP body parsing
### keep-alive
Http without keep-alive useds a new tcp connection every request round, which causes frequent tcp connecting & disconnecting. keep-alive allow multiple http request to use one tcp connection, this connection will kepp a specific duration before disconneting.