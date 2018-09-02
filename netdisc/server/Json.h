#ifndef _JSON_H_
#define _JSON_H_

//json for server
#include <string>

class Json
{
public:
    Json()
    {
    }
    ~Json()
    {
    }
    void Make_User_Json(const std::string item,const std::string data)
    {
        size_t site = json_user.find(item) + item.size() + 3; 
        json_user.insert(site,data);
    }
    void Make_File_Json(const std::string item,const std::string data)
    {
        size_t site = json_file.find(item) + item.size() + 3; 
        json_file.insert(site,data);
    }
private:
    std::string Get_User_Data(const std::string name)
    {
        std::string f;
        for(size_t i = json_user.find(name)+name.size()+3;json_user[i] != '"' || i<json_user.size();i++)
        {
            f += json_user[i];
        }
        return f; 
    }
    std::string Get_File_Data(const std::string name)
    {
        std::string f;
        for(size_t i = json_file.find(name)+name.size()+3;json_file[i] != '"' || i<json_file.size();i++)
        {
            f += json_file[i];
        }
        return f; 
    }
    std::string json_user = R"("User":"","Passwd":"","Regtime":"")";
    std::string json_file = R"("name":"","md5":"","size":"")";
};

#endif
