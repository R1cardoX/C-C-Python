#include <cstdlib>
#include <string>

class Udp
{
private:
    std::string Obtian_Time()
    {
        std::string s_time;
        struct timeval tv;
        long long time = tv.tv_sec*1000000 + tv.tv_usec;
        while(time)
        {
            s_time += time%10+'0';
            time /=10;
        }
        return "[" + s_time + "]";
    }
};
