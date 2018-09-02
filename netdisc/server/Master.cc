#include "Epoll.h"
#include "ThreadPool.h"
#include <map>
//g++ -g -Wall -std=c++11 Master.cc -D_GLIBCXX_USE_NANOSLEEP -lpthread -o Master 

class Master_Server:public Epoll,public ThreadPool
{
public:
    void Init()
    {
        this->Init_Socket();
        this->Init_Poll();
        this->Init_Thead();
    }
private:
    std::list<Connect_Socket> Server_List;
    std::map<std::string,int> Online_User;
    std::map<std::string,int> Connect_User;
    const short port = 8000;
    bool Add_Some_Magic(Connect_Socket cs)
    {
        //client CLIENT:Action:xxxx,User:xxxx,Passwd:xxxx
        //client CLIENT:Action:Register,User:xxxxx
        //client CLIENT:Action:Del_File,User:xxxxx,File_Path:/xxx/xxxxx/xx
        //client CLIENT:Action:Add_File,User:xxxxx,File_Path:/xxxx/xxx/xx,File_Size:xxxxx,MD5:xxxxx
        //client CLIENT:Action:View_File,User:xxxxx
        //server S_MySQL:User_Login/User_Signout,User:xxxx,Name:xxxxx,Condition:Success
        //server S_MySQL:User_Register,User:xxxx,Name:xxxxx,Rand_ID:xxxx
        //server S_MySQL:User_Check,User:xxxx,Condition:Success
        //server S_Files:Add_File,User:xxxxxx,Condition:Success
        //server S_Files:View_File,User:xxxxxx,Files:ssssssssss
        //server S_Files:Del_File,User:xxxxxx,Condition:Success
        while(true)
        {
            std::list<std::string> Read_List = Read(cs.fd);
            Connect_Socket next_connect = cs;
            auto ite = Read_List.begin();
            while(ite != Read_List.end())
            {
                std::string s = *ite;
                if(s.substr(0,5) == "CLIENT")
                {
                    if(Find_Server(Get_Data(s,"Action"),next_connect)) 
                    {
                        Connect_User[Get_Data(s,"User")] = cs.fd;
                        Write(next_connect.fd,s.c_str(),s.size());
                    }
                    else
                    {
                        std::cout<<"Find Server Error"<<std::endl;
                        s = "Master:Server_Not_Found,Say:Server failure,please try again later ...";
                        Write(cs.fd,s.c_str(),s.size());
                    }
                }
                else if(s.substr(0,6) == "S_MySQL")
                {
                    if(s.substr(8,19) == "Server_Login")
                    {
                        Server_List.push_back(cs);
                        s = "Master:Login_Success,Say:Login Success ...";
                        Write(cs.fd,s.c_str(),s.size());
                    }
                    if(s.substr(8,17) == "User_Login")
                    {
                        auto ite = Connect_User.find(Get_Data(s,"User"));
                        int fd = ite->second;
                        if(Get_Data(s,"Condition") == "Success")
                        {
                            Connect_User.erase(ite);
                            Online_User[Get_Data(s,"User")] = fd;
                            s = "Master:Login_Success,Say: Hello " + Get_Data(s,"User") +" ...";
                        }
                        else
                            s = "Master:Wrong_Info,Say:Wrong User Name or Passwd ...";
                        Write(fd,s.c_str(),s.size());
                    }
                    else if(s.substr(8,19) == "User_Signout")
                    {
                        auto ite = Online_User.find(Get_Data(s,"User"));
                        int fd = ite->second;
                        s = "Master:Signout,Say:Signout Already ...";
                        ite = Online_User.erase(ite);
                        Write(fd,s.c_str(),s.size());
                    }
                    else if(s.substr(8,20) == "User_Register")
                    {
                        auto ite = Connect_User.find(Get_Data(s,"Rand_ID"));
                        int fd = ite->second;
                        Connect_User.erase(ite);
                        Online_User[Get_Data(s,"User")] = fd;
                        s = "Master:Register_Success,Say:Register Success and Already Login ...";
                        Write(fd,s.c_str(),s.size()); 
                    }
                    else if(s.substr(8,17) == "User_Check")
                    {
                        auto ite = Connect_User.find(Get_Data(s,"User"));
                        int fd = ite->second;
                        if(Get_Data(s,"Condition") == "Success")
                        {
                            s = "Master:User_Name_Can_Use,Say:OK`S User Name";
                        }
                        else
                        {
                            s = "Master:User_Name_Repeat,Say:User Name is Already Use ,Please Change Again ...";
                        }
                        Write(fd,s.c_str(),s.size());
                    }
                }
                else if(s.substr(0,6) == "S_Files")
                {
                    if(s.substr(8,16) == "View_File")
                    {
                        auto ite = Online_User.find(Get_Data(s,"User"));
                        if(ite != Online_User.end())
                        {
                            int fd = ite->second;
                            s = "Master:Files,Say: " + Get_Data(s,"Files"); 
                            Write(fd,s.c_str(),s.size());
                        }
                    }
                    else if(s.substr(8,15) == "Add_File")
                    {
                        auto ite = Online_User.find(Get_Data(s,"User"));
                        if(ite != Online_User.end())
                        {
                            if(Get_Data(s,"Condition") == "Success")
                            {
                                s = "Master:Begin_Add,Say:Begin Add File";
                                Write(ite->second,s.c_str(),s.size());
                            }
                            else
                            {
                                s = "Master:Error,Say:File's Size To Large ...";
                                Write(ite->second,s.c_str(),s.size());
                            }
                        }
                    }
                    else if(s.substr(8,15) == "Del_File")
                    {
                        auto ite = Online_User.find(Get_Data(s,"User"));
                        if(ite != Online_User.end())
                        {
                            if(Get_Data(s,"Condition") == "Success")
                            {
                                s = "Master:Del_Success,Say:Del File Success";
                                Write(ite->second,s.c_str(),s.size());
                            }
                            else
                            {
                                s = "Master:Error,Say:Not Found This File ...";
                                Write(ite->second,s.c_str(),s.size());
                            }
                        }
                    }
                }
                s.clear();
                ite++;
            }
		}
        return false;
    }
    bool Find_Server_Port(int port,Connect_Socket &next_connect)
    {
            pthread_mutex_lock(&alock);
            for(auto server:Server_List)
            {
                if(server.port == port)
                {
                    next_connect = server;
                    return true;
                }
            }
            pthread_mutex_unlock(&alock);
            return false;
    }
    template <typename Num>
    std::string Num_To_String(Num num)
    {
        std::string n;
        while(num)
        {
            n.insert(n.begin(),num%10+'0');
            num /= 10;
        }
        return n;
    }
    int String_To_Num(std::string num)
    {
        int n = 0;
        int i = 1;
        for(auto rite = num.rbegin(); rite != num.rend();rite++)
        {
            n += (*rite)*i;
            i *= 10;
        }
        return n;
    }
    bool Find_Server(std::string Do,Connect_Socket &next_connect)
    {
        if(Do == "Login" || Do == "Register" || Do == "Check")
        {
            if(Find_Server_Port(10001,next_connect))
            {
                return true;
            }
        }
        else
        {
            if(Find_Server_Port(10002,next_connect))
            {
                return true;
            }
        }
        return false;
    }
    bool Find_Client(std::string user,int &fd)
    {
        auto ite = Online_User.find(user);
        if(ite != Online_User.end())
        {
            fd = ite->second;
            return true;
        }
        else
            return false;
    }
};
int main()
{
    Master_Server server;
    server.Init();
    while(true)
    {
        std::cout<<"wait for client"<<std::endl;
        server.Wait();
        if(server.Is_Add_TaskQueue())
        {
            server.Push_Task(&Master_Server::Server_Work,&server,(void*)server.obtian_sockfd());
        }
    }
    return 0;
}
