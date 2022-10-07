# 测试结果
## 最基础版本http服务器(处理完直接断开)
## Benchmarking: GET http://127.0.0.1:8088/index.html
Webbench - Simple Web Benchmark 1.5
Copyright (c) Radim Kolar 1997-2004, GPL Open Source Software.

Benchmarking: GET http://127.0.0.1:8088/index.html
100 clients, running 30 sec.

Speed=3648462 pages/min, 3405229 bytes/sec.
Requests: 1824231 susceed, 0 failed.

## 完备的Http服务器(不支持长链接和定时器)
[root@ecs-7073 ~]# webbench -c 500 -t 10 http://127.0.0.1:8088/index.html
Webbench - Simple Web Benchmark 1.5
Copyright (c) Radim Kolar 1997-2004, GPL Open Source Software.

Benchmarking: GET http://127.0.0.1:8088/index.html
500 clients, running 10 sec.

Speed=2150262 pages/min, 3798796 bytes/sec.
Requests: 358377 susceed, 0 failed.

## htttp keep-alive
```
[root@ecs-7073 ~]# curl -v http://127.0.0.1:8088/index.html http://127.0.0.1:8088/index.html
*   Trying 127.0.0.1...
* TCP_NODELAY set
* Connected to 127.0.0.1 (127.0.0.1) port 8088 (#0)
> GET /index.html HTTP/1.1
> Host: 127.0.0.1:8088
> User-Agent: curl/7.61.1
> Accept: */*
> 
< HTTP/1.1 200 OK
< Content-Type: text/html
< Content-Length: 37
< Server: FlashWeb Server
< 
Hi,im index.html from FalshWebserver
* Connection #0 to host 127.0.0.1 left intact
* Found bundle for host 127.0.0.1: 0x55e17ad968b0 [can pipeline]
* Re-using existing connection! (#0) with host 127.0.0.1
* Connected to 127.0.0.1 (127.0.0.1) port 8088 (#0)
> GET /index.html HTTP/1.1
> Host: 127.0.0.1:8088
> User-Agent: curl/7.61.1
> Accept: */*
> 
< HTTP/1.1 200 OK
< Content-Type: text/html
< Content-Length: 37
< Server: FlashWeb Server
< 
Hi,im index.html from FalshWebserver
* Connection #0 to host 127.0.0.1 left intact
```
two requests used the same tcp connection

## http keepalive webbench
```
[root@ecs-7073 ~]# webbench -c 100 -t 10 -k -2 http://127.0.0.1:8088/index.html
Webbench - Simple Web Benchmark 1.5
Copyright (c) Radim Kolar 1997-2004, GPL Open Source Software.

Request:
GET /index.html HTTP/1.1
User-Agent: WebBench 1.5
Host: 127.0.0.1
Connection: Keep-Alive


Runing info: 100 clients, running 10 sec.

Speed=4495362 pages/min, 13261318 bytes/sec.
Requests: 749227 susceed, 0 failed.
```

## http without keepalive webbench
```
[root@ecs-7073 ~]# webbench -c 1000 -t 10 http://127.0.0.1:8088/index.html
Webbench - Simple Web Benchmark 1.5
Copyright (c) Radim Kolar 1997-2004, GPL Open Source Software.

Request:
GET /index.html HTTP/1.0
User-Agent: WebBench 1.5
Host: 127.0.0.1


Runing info: 1000 clients, running 10 sec.

Speed=2231988 pages/min, 4687175 bytes/sec.
Requests: 371998 susceed, 0 failed.
```