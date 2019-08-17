//
// Created by ggg1235 on 13/8/2019.
//

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <cerrno>

#define BUF_SIZE 1024
#define EPOLL_SIZE 50

void setnonblockingmode(int fd);
void error_handling(const std::string &message);

int main(int argc,const char **argv,const char **envp){
    int serv_sock,clnt_sock;
    sockaddr_in serv_addr{},clnt_addr{};
    socklen_t clnt_addr_size;

    epoll_event *ep_events;
    epoll_event event{};
    int epfd,event_cnt;

    if (argc!=2){
        printf("Usage : %s <port> \n",argv[0]);
        exit(1);
    }

    serv_sock=socket(PF_INET,SOCK_STREAM,0);
    if (serv_sock==-1){
        error_handling("socket() error!");
    }

    memset(&serv_addr,0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(atoi(argv[1]));

    if (bind(serv_sock,(sockaddr*)&serv_addr, sizeof(serv_addr))==-1){
        error_handling("bind() error!");
    }

    if (listen(serv_sock,5)==-1){
        error_handling("listen() error!");
    }

    epfd=epoll_create(EPOLL_SIZE);
    ep_events=(epoll_event*)malloc(sizeof(epoll_event)*EPOLL_SIZE);

    setnonblockingmode(serv_sock);
    event.events=EPOLLIN;
    event.data.fd=serv_sock;
    epoll_ctl(epfd,EPOLL_CTL_ADD,serv_sock,&event);

    while (1) {
        event_cnt=epoll_wait(epfd,ep_events,EPOLL_SIZE,-1);
        if (event_cnt==-1){
            std::cerr<<"epoll_wait() error!"<<std::endl;
            break;
        }
        std::cout<< "return epoll_wait" << std::endl;
        for (int i = 0; i < event_cnt ; i++) {
            if (ep_events[i].data.fd==serv_sock){
                clnt_addr_size= sizeof(clnt_addr);
                clnt_sock=accept(serv_sock,(sockaddr*)&clnt_addr,&clnt_addr_size);
                if (clnt_sock==-1){
                    error_handling("accept() error!");
                } else {
                    std::cout<<"Accept "<< i+1 << "...... " <<std::endl;
                }
                setnonblockingmode(clnt_sock);
                event.events=EPOLLIN|EPOLLET;
                event.data.fd=clnt_sock;
                epoll_ctl(epfd,EPOLL_CTL_ADD,clnt_sock,&event);
                std::cout<< "Connected client: " << clnt_sock << std::endl;
            } else {
                while(1) {
                    char buf[BUF_SIZE];
                    int str_len = recv(ep_events[i].data.fd, buf, BUF_SIZE, 0);

                    if (str_len == 0) {
                        epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, nullptr);
                        close(ep_events[i].data.fd);
                        std::cout << "Closed client: " << ep_events[i].data.fd << std::endl;
                    }else if(str_len<0){
                        if (errno==EAGAIN){
                            break;
                        }
                    } else {
                        send(ep_events[i].data.fd, buf, str_len + 1, 0);
                    }
                }
            }
        }
    }
    close(serv_sock);
    close(epfd);
    return 0;
}

void error_handling(const std::string &message){
    std::cerr<<message<<std::endl;
    exit(1);
}

void setnonblockingmode(int fd){
    int flag=fcntl(fd,F_GETFL,0);
    fcntl(fd,F_SETFL,flag|O_NONBLOCK);
}