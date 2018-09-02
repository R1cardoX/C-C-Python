#ifndef _SOCKET_H
#define _SOCKET_H_

#include <iostream>
#include <mutex>
#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <list>
#include <sys/socket.h>
#include <sys/mman.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netinet/in.h>
#include <ctype.h>
#include <vector>
#include <errno.h>
#include <signal.h>
#include <sys/epoll.h>

#define outStr(x) std::cout<<x<<std::endl; 

struct Connect_Socket
{
    int fd;
    struct sockaddr_in Sockaddr;
    char *ip;
    int port;
    int Addr_Size;
};

pthread_mutex_t alock = PTHREAD_MUTEX_INITIALIZER;
class Epoll
{
public:
    const short port = 8000;
    const short buf_size = 1500;
    const short listen_num = 128;
    const short ip_size = 16;
private:
    long int socketfd;
    int epoll_fd;
    struct epoll_event evt;
    struct epoll_event evearr[1];
public:
    Epoll()
    {

    }
    ~Epoll()
    {

    }
    void Init_Poll(int num = 10)
    {
        //Daemon();
        this->epoll_fd = epoll_create(num);
        this->evt.data.fd = this->socketfd;
        this->evt.events = EPOLLIN;
        this->evt.events |= EPOLLET;
        epoll_ctl(this->epoll_fd,EPOLL_CTL_ADD,this->socketfd,&(this->evt));
    }
    void Init_Socket()
    {
        sockaddr_in Server_Addr;
        int fd;
        memset(&Server_Addr,0,sizeof(Server_Addr));
        Server_Addr.sin_family = AF_INET;
        Server_Addr.sin_port = htons(port);
        Server_Addr.sin_addr.s_addr = INADDR_ANY;
        fd = socket(AF_INET,SOCK_STREAM,0);
        if((bind(fd,(struct sockaddr *)&Server_Addr,sizeof(Server_Addr)))==-1)
		{
            std::cerr<<"Bind Error"<<std::endl;
            exit(0);
		}
		listen(fd,listen_num);
        this->socketfd = fd;
        outStr("Init Socket Success ...");
    }
    int Wait()
    {
        return epoll_wait(this->epoll_fd,this->evearr,1,-1);
    }
    bool Is_Add_TaskQueue()
    {
        return this->evearr[0].data.fd == this->socketfd;
    }
    void* Server_Work(void* fd)
    {
        std::cout<<"Begin Server Work ..."<<std::endl;
        int sockfd = (long int)fd;
		int client,clientsize;
		struct sockaddr_in clientaddr;
		char ipstr[ip_size];
		clientsize = sizeof(clientaddr);
        pthread_mutex_lock(&alock);
        std::cout<<"tid:"<<pthread_self()<<" Accepting....     ";
		client = accept(sockfd,(struct sockaddr *)&clientaddr,(socklen_t*)&clientsize);
        pthread_mutex_unlock(&alock);
        if(client < 0)
        {
            std::cout<<"Accept Error ..."<<std::endl;
        }
        printf("tid:%x  ip:%s  port:%d\n",(unsigned int)pthread_self(),inet_ntop(AF_INET,&clientaddr.sin_addr.s_addr,ipstr,ip_size),ntohs(clientaddr.sin_port));
        Connect_Socket CS;
        CS.ip = ipstr;
        CS.port = ntohs(clientaddr.sin_port);
        CS.fd = client;
        CS.Addr_Size = clientsize;
        CS.Sockaddr = clientaddr;
        if(!Add_Some_Magic(CS))
            close(client);
        outStr("CLient Close ...");
		return NULL;
    }
    virtual bool Add_Some_Magic(Connect_Socket) = 0;
    long int obtian_sockfd()
    {
        return this->socketfd;
    }
    std::list<std::string> Read(int fd)
    {
        std::list<std::string> Read_List;
        std::string msg;
        char buf[buf_size];
        std::string Begin = "[Begin][";
        std::string End = "][End]";
        read(fd,buf,buf_size);
        std::string b = buf;
        while(true)
        {
            size_t i = b.find_first_of(Begin) + Begin.size(); 
            if(b.find_first_of(End) == i)
            {
                Read_List.push_back(msg);
                msg.clear();
                b = b.substr(b.find_first_of(End)+End.size(),b.size()-1);
                if(b.size() == 0)
                    break;
            }
            msg.push_back(b[i]);
        }
        return Read_List;
    }
    void Write(int fd,const char *buf,int len)
    {
        std::string Begin = "[Begin][";
        std::string End = "][End]";
        std::string b = Begin + buf + End;
        b.resize(len + End.size() + Begin.size());
        write(fd,b.c_str(),b.size());
    }
    std::string Get_Data(std::string msg,const std::string type)
    {
        std::string result;
        for(size_t i = msg.find(type) + type.size() + 1;i<msg.size() && msg[i] != ',';i++)
        {
            result.push_back(msg[i]);
        }
        return result;
    }
private:
    void Daemon()
    {
		pid_t pid;
		pid = fork();
		if(pid > 0)
        {
            exit(0);
		}
        else if(pid == 0)
        {
            setsid();
            chdir("/");
            umask(0);
            open("/tmp/thread_pool.log",O_RDWR|O_CREAT,0664);
            printf("Create It!\n");
		}
        else
        {
            perror("fork error:");
		}
    }
};

#endif
