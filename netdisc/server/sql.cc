#include "MySQL.h"


int main()
{
    int port = 33060;
    std::string user = "root";
    std::string pwd = "19960419jh";    
    MySQL mysql(port,user,pwd);
    std::cout <<"Session accepted, creating collection..." <<std::endl;
#if 1
    mysql.Add(R"({ "name": "foo", "age": 1 })",true);
    mysql.Add(R"({ "name": "bar", "age": 2, "toys": [ "car", "ball" ] })",true);
    mysql.Add(R"({
       "name": "baz",
        "age": 3,
       "date": { "day": 20, "month": "Apr" }
    })",true);
    mysql.Add(R"({ "_id": "myuuid-1", "name": "foo", "age": 7 })",true);
    std::cout <<"Fetching documents..." <<std::endl;
    mysql.Find("age > 1 and name like 'ba%'");
    std::string str = R"("
    {
        "id":0000000,
        "user":"_user",
        "pw":"_pw",
        "vip":0,
        "file":"/file",
        "music":"_music",
        "video":"_video",
        "picture":"_picture",
        "zip":"_zip", 
        "recycle":"_recycle",
        "file_size":"_fsize",
        "recycle_size":"_rsize"
    }")";
#endif

}

