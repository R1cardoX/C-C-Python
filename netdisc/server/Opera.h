#include <string>
#include <vector>
#include <iostream>
#define Debug(x) std::cerr<<(#x)<<" : "<<x<<std::endl;

class Opera
{
public:
    std::string Add(std::string a,std::string b)
    {
        std::string result;
        int flag = 0;
        std::string::reverse_iterator rite_a = a.rbegin();
        std::string::reverse_iterator rite_b = b.rbegin();
        while(rite_a != a.rend() || rite_b != b.rend())
        {
            int _a,_b;
            if(rite_a != a.rend())
            {
                _a = *rite_a-'0';
                rite_a++;
            }
            else
            {
                _a = 0;
            }
            if(rite_b != b.rend())
            {
                _b = *rite_b-'0';
                rite_b++;
            }
            else
            {
                _b = 0;
            }
            if(_a + _b + flag < 10)
            {
                result.insert(result.begin(),_a+_b+'0'+flag);
                flag = 0;
            }
            else if(_a + _b + flag < 20)
            {
                result.insert(result.begin(),_a+_b-10+'0'+flag);
                flag = 1;
            }
        }
        if(flag)
            result.insert(result.begin(),'1');
        return result;
    }
    std::string Mount(std::string a,std::string b)
    {
        std::vector<std::string> pre_result;
        std::string result;
        int num = 0,n=0;
        std::string::reverse_iterator rite_a = a.rbegin();
        std::string::reverse_iterator rite_b = b.rbegin();
        while(rite_b != b.rend())
        {
            int _a,_b,flag = 0;;
            std::string r = "";
            _b = *rite_b-'0';
            rite_a = a.rbegin();
            while(rite_a != a.rend())
            {
                _a = *rite_a-'0';
                num = (_a * _b + flag)%10;
                flag = (_a * _b + flag)/10;
                r.insert(r.begin(),num+'0');
                rite_a++;
            }
            for(int i=0;i<n;i++)
                r.push_back('0');
            if(flag)
                r.insert(r.begin(),flag+'0');
            pre_result.push_back(r);
            r.clear();
            n++;
            rite_b++;
        }      
        result = pre_result[0];
        for(size_t i=1;i<pre_result.size();i++)
        {
            result = Add(result,pre_result[i]);
        }
        return result;
    }
    std::string Sub(std::string a,std::string b)
    {
        std::string result;
        if(a.size() < b.size())
        {
            std::string t;
            t = a;
            a = b;
            b = a;
        }
        int flag=0;
        std::string::reverse_iterator rite_a = a.rbegin();
        std::string::reverse_iterator rite_b = b.rbegin();
        while(rite_b != b.rend())
        {
            int num = *rite_a - *rite_b + flag;
            if(num< 0)
            {
                num += 10;
                flag = -1;
            }
            result.insert(result.begin(),num+'0');  
            rite_a++;
            rite_b++;
        }
        while(rite_a != a.rend())
        {
            int num = *rite_a - '0' - flag;
            if(num < '0')
            {
                num += 10;
                flag = -1;
            }
            result.insert(result.begin(),num + '0'); 
            rite_a++;
        }
        if(flag == -1)
            result.insert(result.begin(),'-');
        return result;
    }
};
