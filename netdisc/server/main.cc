#include <iostream>
#include <string>
#include <chrono>
#include "ThreadPool.h"
#include "Epoll.h"
#include "RSA.h"
#include "MySQL.h"
#include "MD5.h"
//g++ -g -Wall main.cpp -std=c++11 -D_GLIBCXX_USE_NANOSLEEP -lpthread -o server

#define ar(x) do{std::cout<<x<<std::endl;}while(0)


class MyServer:public Epoll,public ThreadPool,public MySQL
{
private:
    void Do_Some_Thing(std::string str)
    {
        FOR(ite,str.begin(),str.end())
            *ite = toupper(*ite);
    }
    std::string Obtian_MD5(const std::string &file)//文件路径
    {
        std::ifstream in(file.c_str(), std::ios::binary);
        if (!in)
            return "路径错误";
        MD5 md5(in);
        md5.update(in);
        in.close();
        return md5.To_Sting();
    }
};



int main()
{
#if 0
    MyServer server;
    server.Init_Thead();
    server.Init();
    ar("init epoll");
    server.Init_Socket();
    ar("init socket");
    while(true)
    {
        server.Wait_Epoll();
        ar("wait ...");
        if(server.Is_Add_TaskQueue())
        {
            server.Push_Task(&Server::Server_Work,&server,server.obtian_sockfd());
        }
    }
    RSA rsa;
    std::string s,c;
    s = "123456789";
    rsa.encode(s);
#endif
    return 0;
}
