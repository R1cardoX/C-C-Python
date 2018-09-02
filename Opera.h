#include <string>
#include <cstdio>
#include <stdlib.h>
#include <list>
#include <queue>
#include <memory>
#include <stack>
#include <vector>
#include <iostream>
#define Debug(x) do{std::cerr<<(#x)<<":"<<x<<std::endl;}while(0)
#define abs(x)  ((x)>0?(x):-(x))

class Opera
{
private:
    Opera()
    {
    }
    Opera(Opera&)
    {
    }
    ~Opera()
    {
    }
    void operator=(Opera&)
    {
    }
    template <typename T>
    static bool Is_Large(T a,T b)
    {
        if(a.size() > b.size())
            return true;
        else if(a.size() < b.size())
            return false;
        auto ite_a = a.begin();
        auto ite_b = b.begin();
        while(ite_a != a.end() || ite_b != b.end())
        {
            if(*ite_a > *ite_b)
                return true;
            else if(*ite_b > *ite_a)
                return false;
            else
            {
                ite_a++;
                ite_b++;
            }
        }
        return false;
    }
    template <typename T>
    static std::list<T> Obtian_List(T b)
    {
        abs(b);
        std::list<T> temp;
        while(b)
        {
            temp.push_front(b%10);
            b /= 10;
        }
        return temp;
    }
    template <typename T>
    static std::string Obtian_String(T b)
    {
        std::string temp;
        while(b)
        {
            temp = char(b%10+'0') + temp;
            b /= 10;
        }
        return temp;
    }
    template <typename T>
    static void Swap(T &a,T &b)
    {
        T temp(std::move(a));
        a = std::move(b);
        b = std::move(temp);
    }
    template <typename T>
    static void To_Num(T &a)
    {
        if(a >= '0' && a <= '9')
            a -= '0';
    }
public:
    template <typename T>
    static auto Sum(T a,T b) -> decltype(a)
    {
        T result;
        std::stack<int> num; 
        int site,temp = 0,flag = 0;
        if(a.size() < b.size())
            Swap(a,b);
        auto rite_a = a.rbegin();
        auto rite_b = b.rbegin();
        if(*rite_a >= '0' && *rite_a <= '9')
            temp = 1;
        while(rite_b != b.rend())
        {
            To_Num(*rite_a);
            To_Num(*rite_b);//如果是字符型把他转换成整形
            site = *rite_a+*rite_b+flag;
            num.push(site%10); 
            flag = site >= 10 ? 1 : 0;
            rite_a++;
            rite_b++;
        }
        while(rite_a != a.rend())
        {
            To_Num(*rite_a);
            site = *rite_a + flag;
            num.push(site);
            flag =  site >= 10 ? 1 : 0; 
            rite_a++;
        }
        if(flag != 0)
            num.push(flag);
        while(!num.empty())
        {
            site = num.top();
            num.pop();
            result.push_back(site+'0'*temp);
        }
        return result;
    }
    template <typename T>
    static auto Mount(T a,T b) -> decltype(a)
    {
        T result;
        std::stack<int> num; 
        std::list<T> num_lst;
        int site,count = 0,temp = 0,flag = 0;
        if(a.size() < b.size())
            Swap(a,b);
        auto rite_a = a.rbegin();
        auto rite_b = b.rbegin();
        if(*rite_a >= '0' && *rite_a <= '9')
            temp = 1;
        while(rite_a != a.rend())
        {
            for(int i=0;i<count;i++)
                num.push(0);
            count++;
            rite_b = b.rbegin();
            while(rite_b != b.rend())
            {
                To_Num(*rite_a);
                To_Num(*rite_b);//如果是字符型把他转换成整形
                site = (*rite_a) * (*rite_b) + flag; 
                flag = site/10;
                num.push(site%10);
                rite_b++;
            }
            if(flag != 0)
                num.push(flag);
            while(!num.empty())
            {
                site = num.top();
                num.pop();
                result.push_back(site+'0'*temp);
            }
            num_lst.push_back(result);
            result.clear();
            rite_a++;
        }
        for(auto var:num_lst)
            result = Sum(var,result);
        return result;
    }
    template <typename T>
    static auto Sub(T a,T b) -> decltype(a)
    {
        T result;
        std::stack<int> num; 
        int site,temp = 0,flag = 0;
        if(a.size() < b.size())
            Swap(a,b);
        auto rite_a = a.rbegin();
        auto rite_b = b.rbegin();
        if(a.back() >= '0' && a.back() <= '9')
            temp = 1;
        while(rite_b != b.rend())
        {
            To_Num(*rite_a);
            To_Num(*rite_b);//如果是字符型把他转换成整形
            site = *rite_a - *rite_b + flag; 
            site < 0 ? site+=10,flag=-1:flag = 0;
            num.push(site);
            rite_a++;
            rite_b++;
        }
        while(rite_a != a.rend())
        {
            To_Num(*rite_a);
            site = *rite_a + flag;
            site < 0 ? site+=10,flag=-1:flag = 0;
            num.push(site);
            rite_a++;
        }
        bool f = 0;
        while(!num.empty())
        {
            site = num.top();
            if(site == 0 && f == 0 && num.size() != 1)
            {
                num.pop();
                continue;
            }
            else
            {
                f = 1;
            }
            num.pop();
            result.push_back(site+'0'*temp);
        }
        return result;
    }
    template <typename T>
    static auto Div(T a,T b) -> decltype(a)// a/b
    {
        T result;
        if((a.size() < b.size())||(a.size() == b.size() && *a.begin() < *b.begin()))
        {
            result = "0";
            return result;
        }
        int site=1,temp = 0;
        int dif = a.size()-b.size();
        if(a.back() >= '0' && a.back() <= '9')
            temp = 1;
        bool f = false;
        while(dif >= 0)
        {
            T str = b;
            for(int i=0;i<dif;i++)
            {
                str.push_back('0'*temp);
            }
            str.resize(b.size()+dif);
            site=1;
            while(!Is_Large(str,a))
            {
                a = Sub(a,str);
                site++;
            }
            if(site == 1 && f == false)
            {
                dif--;
                continue;
            }
            else
            {
                f = true;
            }
            site = site + '0'*temp - 1;
            result.push_back(site);
            dif--;
        }
        return result;
    }
    static const char* Mount(const char* str1,const char* str2)
    {
        std::string a = str1;
        std::string b = str2;
        std::unique_ptr<std::string> c(new std::string(Mount(a,b)));
        return c->c_str();
    }
    static const char* Sum(const char* str1,const char* str2)
    {
        std::string a = str1;
        std::string b = str2;
        std::unique_ptr<std::string> c(new std::string(Sum(a,b)));
        return c->c_str();
    }
    static const char* Sub(const char* str1,const char* str2)
    {
        std::string a = str1;
        std::string b = str2;
        std::unique_ptr<std::string> c(new std::string(Sub(a,b)));
        return c->c_str();
    }
    static const char* Div(const char* str1,const char* str2)
    {
        std::string a = str1;
        std::string b = str2;
        std::unique_ptr<std::string> c(new std::string(Div(a,b)));
        return c->c_str();
    }
    template <typename T>
    static auto Sum(T a,long long b) -> decltype(a)
    {
        std::list<long long> temp = Obtian_List(b);
        if(a.back() <= '9' && a.back() >= '0')
        {
            for(auto &var:temp)
            {
                var += '0';
            }
        }
        T bb(temp.begin(),temp.end());
        return Sum(a,bb);
    }
    template <typename T>
    static auto Mount(T a,long long b) -> decltype(a)
    {
        std::list<long long> temp = Obtian_List(b);
        if(a.back() <= '9' && a.back() >= '0')
        {
            for(auto &var:temp)
            {
                var += '0';
            }
        }
        T bb(temp.begin(),temp.end());
        return Mount(a,bb);
    }
    template <typename T>
    static auto Sub(T a,long long b) -> decltype(a)
    {
        std::list<long long> temp = Obtian_List(b);
        if(a.back() <= '9' && a.back() >= '0')
        {
            for(auto &var:temp)
            {
                var += '0';
            }
        }
        T bb(temp.begin(),temp.end());
        return Sub(a,bb);
    }
    template <typename T>
    static auto Div(T a,long long b) -> decltype(a)
    {
        std::list<long long> temp = Obtian_List(b);
        if(a.back() <= '9' && a.back() >= '0')
        {
            for(auto &var:temp)
            {
                var += '0';
            }
        }
        T bb(temp.begin(),temp.end());
        return Div(a,bb);
    }
    template <typename T>
    static auto Mount(long long a,T b) -> decltype(b)
    {
        return Mount(b,a);
    }
    template <typename T>
    static auto Sum(long long a,T b) -> decltype(b)
    {
        return Sum(b,a);
    }
    template <typename T>
    static auto Sub(long long a,T b) -> decltype(b)
    {
        return Sub(b,a);
    }
    template <typename T>
    static auto Div(long long a,T b) -> decltype(b)
    {
        return Div(b,a);
    }
    static std::string Sum(long long a,long long b)
    {
        return Sum(Obtian_String(a),Obtian_String(b));
    }
    static std::string Sub(long long a,long long b)
    {
        return Sub(Obtian_String(a),Obtian_String(b));
    }
    static std::string Div(long long a,long long b)
    {
        return Div(Obtian_String(a),Obtian_String(b));
    }
    static std::string Mount(long long a,long long b)
    {
        return Mount(Obtian_String(a),Obtian_String(b));
    }
};

int main()
{
    std::cout<<Opera::Div("987654321","12345");
}

