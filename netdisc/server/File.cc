#include "File.h"
#include "ThreadPool.h"
#include "Epoll.h"

//client CLIENT:Action:Del_File,User:xxxxx,File_Path:/xxx/xxxxx/xx
//client CLIENT:Action:Add_File,User:xxxxx,File_Path:/xxxx/xxx/xx,File_Size:xxxxx,MD5:xxxxx
//client CLIENT:Action:View_File,User:xxxxx
//server S_Files:Add_File,User:xxxxxx,Condition:Success
//server S_Files:View_File,User:xxxxxx,Files:ssssssssss
//server S_Files:Del_File,User:xxxxxx,Condition:Success
class File_Server:public ThreadPool,public File,public Epoll
{
public:
    void Init()
    {
        this->Init_Socket();
        this->Init_Poll();
        this->Init_Thead();
    }
private:
    const short port = 10002;
    bool Add_Some_Magic(Connect_Socket cs)
    {
        while(true)
        {
            std::list<std::string> Read_List = Read(cs.fd);
            auto ite = Read_List.begin();
            while(ite != Read_List.end())
            {
                std::string s = *ite;
                std::string user = Get_Data(s,"User");
                File* usr_file = new File(user);
                if(Get_Data(s,"Action") == "View_File")
                {
                    s = "S_Files:View_File,User:" + user + ",Files:"+usr_file->Obtian_Path();
                }
                delete(usr_file);
            }
		}
        return true;
    }
};

int main()
{
    char s[100];
    memset(s,0,sizeof(s));
    getcwd(s,100);
    std::cout<<s;
    File f("..");
    std::cout<<"Path:"<<f.Obtian_Path()<<std::endl;
    std::cout<<"Size:"<<f.Obtian_Size()<<std::endl;
}
