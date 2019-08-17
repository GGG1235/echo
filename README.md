# echo
Linux下基于epoll的echo服务端客户端

---

#### 与select相比epoll的优点
1、不需要编写监视所有文件描述符的循环

2、不需要每次传递监视对象信息

---

socket->bind->listen->accept->read/write->close

accept创建的套接字改成非堵塞,EPOLLIN添加EPOLLET,将套接字事件注册方式改为边缘触发
```
setnonblockingmode(clnt_sock);
event.events=EPOLLIN|EPOLLET;
```

##### 截图:

服务端

![服务端](https://github.com/GGG1235/echo/blob/master/img/server.png)

客户端

![客户端](https://github.com/GGG1235/echo/blob/master/img/client.png)
