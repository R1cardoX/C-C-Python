#ifndef _RSA_H_
#define _RSA_H_

#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <array>
#include <fstream>

#define Debug(x) std::cout<<(#x)<<" : "<<x<<std::endl;
typedef long long ll;

class RSA
{
public:
    std::string encode(std::string str)
    {
        std::string s = "";
        for(size_t i=0;i<str.size();i++)
            s += Quick_Pow(str[i],e,n);
        return s;
    }
    std::string decode(std::string str)
    {
        std::string s="";
        std::stringstream ss(str);
        int x;
        while(ss >> x)
            s += Quick_Pow(x,d,n); 
        std::cout<<s<<std::endl;
        return s;
    }
    RSA()
    {
        Init_Prime();
        srand((unsigned)time(NULL));
        int p1,p2;
        while(true)
        {
            p1 = rand()%MAX;
            p2 = rand()%MAX;
            if(aPrime[p1]&&p1>2&&p2>2&&aPrime[p2])
                break;
        }
        n = p1*p2;
        ll m = (p1-1)*(p2-1);
        ll y;
        for(ll i=3;i<m;i+=1331)
        {
            ll gcd = exgcd(i,m,d,y);
            if(gcd == 1 && d>0)
            {
                e = i;
                break;
            }
        }
        std::cout<<"Public  Key: "<<e<<" , "<<n<<std::endl;
        std::cout<<"Private Key: "<<d<<" , "<<n<<std::endl;
    }
    ~RSA()
    {

    }
private:
    std::array<bool,1001> aPrime;
    const int MAX = 1000;
    ll n,e,d;
    std::ifstream fin;
    std::ofstream fout;
    template <typename A,typename B,typename C>
    ll Quick_Pow(A a,B b,C c)
    {
        //HACK->string 大数据处理
        ll tot = 1;
        while(b)
        {
            if(b&1)
                tot = (1LL*tot*a)%c;
            a = (1LL*a*a)%c;
            b >>= 1;
        }
        return tot;
    }
    template <typename A,typename B,typename X,typename Y>
    auto exgcd(A a,B b,X &x,Y &y) -> decltype(b*a)
    {
        if(b == 0)
        {
            x = 1;
            y = 0;
            return a;
        }
        auto ans = exgcd(b,a%b,x,y);
        X tmp = x;
        x = y;
        y = tmp - a/b * y;
        return ans;
    }
    void Init_Prime()
    {
        for(auto &a:aPrime)
            a = true;
        aPrime[0] = aPrime[1] = false;
        for(size_t i=2;i*i<aPrime.size();i++)
        {
            if(aPrime[i])
            {
                int j = i*i;
                while(j<=(int)aPrime.size())
                {
                    aPrime[j] = false;
                    j+=i;
                }
            }
        }
    }
};

#endif

