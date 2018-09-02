#include "json.h"
#include <iostream>

int main()
{
    std::string str = R"("
    {
        "id":"0123456789",
        "user":"_user",
        "pw":"_pw",
        "vip":0,
        "file":"_file",
        "music":"_music",
        "video":"_video",
        "picture":"_picture",
        "zip":"_zip", 
        "recycle":"_recycle",
        "file_size":"_fsize",
        "recycle_size":"_rsize"
    }")";
    Json json(str);
    std::cout<<json.Obtian_Id()<<std::endl;
    std::cout<<json.Obtian_Pw()<<std::endl;
    std::cout<<json.Obtian_Zip()<<std::endl;
    std::cout<<json.Obtian_File()<<std::endl;
    std::cout<<json.Obtian_User()<<std::endl;
    std::cout<<json.Obtian_Music()<<std::endl;
    std::cout<<json.Obtian_Video()<<std::endl;
    std::cout<<json.Obtian_Picture()<<std::endl;
    std::cout<<json.Obtian_Recycle()<<std::endl;
    std::cout<<json.Obtian_File_Size()<<std::endl;
    std::cout<<json.Obtian_Recycle_Size()<<std::endl;
}
