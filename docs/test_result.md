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

