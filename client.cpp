//
// Created by ggg1235 on 17/8/2019.
//

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

void read_routine(int sock,char *buf);
void write_routine(int sock);
void error_handling(const std::string &message);

int main(int argc,const char **argv,const char **envp){
    int sock;
    sockaddr_in serv_addr{};

    if (argc!=3){
        printf("Usage : %s <IP> <Port> \n",argv[0]);
        exit(1);
    }

    sock=socket(PF_INET,SOCK_STREAM,0);
    if (sock==-1){
        error_handling("socket() error!");
    }

    memset(&serv_addr,0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_addr.sin_port=htons(atoi(argv[2]));

    if (connect(sock,(sockaddr*)&serv_addr, sizeof(serv_addr))==-1){
        error_handling("connect() error!");
    }

//    char buf[BUF_SIZE];
//    pid_t pid=fork();
//    if (pid==0){
//        write_routine(sock);
//    } else {
//        read_routine(sock,buf);
//    }
    int num=5;
    while(num--){
        std::string message;
        getline(std::cin,message);
        if(message=="q" || message=="Q"){
            break;
        }
        int recv_len=0;
        int str_len=send(sock,message.c_str(), message.length()+1,0);
        char words[BUF_SIZE];

        while(recv_len<str_len) {
            int recv_cnt=recv(sock, &words[recv_len], sizeof(words), 0);
            if(recv_cnt==-1){
                error_handling("recv error!");
            }
            recv_len+=recv_cnt;
        }
        std::string m(words);
        std::cout<<"echo:"<<m<<std::endl;
    }
    close(sock);
    return 0;
}

void read_routine(int sock,char *buf){
    int num=5;
    while(num--){
        int str_len=recv(sock,buf,BUF_SIZE,0);
        if(str_len==0){
            return;
        }
        buf[str_len]=0;
        std::string message(buf);
        std::cout<<"echo:"<<message<<std::endl;

    }
}

void write_routine(int sock){
    int num=5;
    while(num--){
        std::string message;
        getline(std::cin,message);
        if(message=="Q"||message=="q"){
            shutdown(sock,SHUT_WR);
            return;
        }
        send(sock,message.c_str(),message.length()+1,0);
    }
}


void error_handling(const std::string &message){
    std::cerr<< message << std::endl;
    exit(1);
}