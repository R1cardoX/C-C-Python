#ifndef _FILE_H_
#define _FILE_H_

#include <string>
#include <unistd.h>
#include <dirent.h>
#include <vector>
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
 
typedef long long ll;
 

class File
{
public:
    File(std::string str)
    {
        user = str;
    }
    std::string Obtian_Path()
    {
        Make_Path_Str();
        return Path;
    }
    ll Obtian_Size()
    {
        return size;
    }
    bool Add_File(std::string path)
    {
        int site = path.find_last_of('/');
        makeDir(path.substr(0,site-1).c_str());
        if(creat(path.c_str(),0665)>0)
            return true;
        else
            return false;
    }
    bool Del_File(std::string path)
    {
        if(remove(path.c_str()) == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
private:
    int makeDir(const char* path)
    {
        int beginCmpPath;
        int endCmpPath;
        int pathLen = strlen(path);
        char currentPath[128] = {0};
     
        printf("path = %s\n", path);
        if('/' != path[0])
        {   
            getcwd(currentPath, sizeof(currentPath));
            strcat(currentPath, "/");
            printf("currentPath = %s\n", currentPath);
            beginCmpPath = strlen(currentPath);
            strcat(currentPath, path);
            if(path[pathLen] != '/')
            {
                strcat(currentPath, "/");
            }
            endCmpPath = strlen(currentPath);
            
        }
        else
        {
            int pathLen = strlen(path);
            strcpy(currentPath, path);
            if(path[pathLen] != '/')
            {
                strcat(currentPath, "/");
            }
            beginCmpPath = 1;
            endCmpPath = strlen(currentPath);
        }
        for(int i = beginCmpPath; i < endCmpPath ; i++ )
        {
            if('/' == currentPath[i])
            {
                currentPath[i] = '\0';
                if(access(currentPath,0) != 0)
                {
                    if(mkdir(currentPath, 0755) == -1)
                    {
                        printf("currentPath = %s\n", currentPath);
                        perror("mkdir error %s\n");
                        return -1;
                    }
                }
                currentPath[i] = '/';
            }
        }
        return 0;
    }
    void Make_Path_Str()
    {
        Init_Path("./user/" + user);
        for(auto p:path)
        {
            Path += p + '\n';
        }
    }
    ll Get_Size(std::string p)
    {
        ll fsize = -1;
        struct stat statbuff;
        if(stat(p.c_str(),&statbuff) < 0)
            return fsize;
        else 
            fsize = statbuff.st_size;
        return fsize;
    }
    void Init_Path(std::string Base_Path)
    {
        DIR *dir = nullptr;
        struct dirent *ptr = nullptr;
        std::string base;
        if((dir = opendir(Base_Path.c_str())) == nullptr )
        {
            std::cerr<<"user not found "<<user<<"..."<<std::endl;
            exit(1);
        } 
        while((ptr = readdir(dir)) != nullptr)
        {
            if(strcmp(ptr->d_name,".") == 0 || strcmp(ptr->d_name,"..") == 0)
                continue;
            else if(ptr->d_type == 8)
            {
                std::string p = Base_Path + "/" + ptr->d_name;
                path.push_back(p);
                ll s = Get_Size(p); 
                if(s != -1)
                {
                    size += s;
                }
            }
            else if(ptr->d_type == 4)
            {
                std::string p = ptr->d_name;
                Init_Path(Base_Path + "/" + p);
            }
        }
        closedir(dir);
    }
    std::string user;
    std::string Path;
    std::vector<std::string> path;
    ll size;
};

#endif
