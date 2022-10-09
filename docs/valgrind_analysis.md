<!--
 * @Version: 
 * @Author: LiYangfan.justin
 * @Date: 2022-10-08 14:08:53
 * @LastEditTime: 2022-10-09 21:16:10
 * @Description: 
 * Copyright (c) 2022 by Liyangfan.justin, All Rights Reserved. 
-->
# valgrind
the valrind report are as follows:  

- [valgrind_report_before_optimization](./valgrind_report_before.log)
- [valgrind_report_after_optimization](./valgrind_report_after.log)

Most leaks are cause by this issue:  
forget to delete main eventloop object(should have used smart pointer)
```
EventLoop* eventloop = new EventLoop();
Server server(eventloop,8088,3);
server.start();
server.shutdown();
delete eventloop;
```