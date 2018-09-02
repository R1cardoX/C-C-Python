#ifndef MD5_H  
#define MD5_H  
 
#include <string>  
#include <string.h>
#include <array>
#include <cstdlib>
#include <fstream>  
#define S11 7  
#define S12 12  
#define S13 17  
#define S14 22  
#define S21 5  
#define S22 9  
#define S23 14  
#define S24 20  
#define S31 4  
#define S32 11  
#define S33 16  
#define S34 23  
#define S41 6  
#define S42 10  
#define S43 15  
#define S44 21  


typedef unsigned char byte;
typedef unsigned long ulong;
class MD5
{
public:
    MD5()
    {
        Re_Set(); 
    }
    MD5(const void*input,size_t nLength)
    {
        Re_Set();
        update(input,nLength);
    }
    MD5(const std::string &str)
    {
        Re_Set();
        update(str);
    }
    MD5(std::ifstream &in)
    {
        Re_Set();
        update(in);
    }
    void update(const void *input,size_t nLength)
    {
        update((const byte*)input,nLength);
    }
    void update(const std::string &str)
    {
        update((const byte*)str.c_str(),str.length());
    }
    void update(std::ifstream &in)
    {
        if(!in)
        {
            return;
        }
        std::streamsize length;
        char Buffer[BufSize];
        while(!in.eof())
        {
            in.read(Buffer,BufSize);
            length = in.gcount();
            if(length > 0)
                update(Buffer,length);
        }
        in.close();
    }
    std::string To_Sting()
    {
        return bytes_to_hexstring(digest().data(), 16);  
    }
    void Re_Set()
    {
        finished = false;
        Count[0] = Count[1] = 0;
        State[0] = 0x67452301;
        State[1] = 0xefcdab89;
        State[2] = 0x98badcfe;
        State[3] = 0x10325476;
    }
    const std::array<byte,64> digest()
    {
        if(!finished)
        {
            finished = true;
            final();
        }
        return Digest;
    }
private:
    static const std::array<byte,64> Padding;
    static const std::array<char,16> Hex;
    static const size_t BufSize;
    std::array<ulong,4> State;
    std::array<ulong,2> Count;
    std::array<byte,64> Buffer;
    std::array<byte,64> Digest;
    bool finished;

private:
    template <typename T>
    inline auto F(T x,T y,T z) -> decltype((x&y)|((~x)&z))
    {
        return ( x & y ) | ((~x) & z);
    }
    template <typename T>
    inline auto G(T x,T y,T z) -> decltype((x&z)|(y&(~z)))
    {
        return ( x & z ) | (y & (~z));
    }
    template <typename T>
    inline auto H(T x,T y,T z) -> decltype(x^y^z)
    {
        return x^y^z; 
    }
    template <typename T>
    inline auto I(T x,T y,T z) -> decltype(y^(x|(~z)))
    {
        return y ^ ( x | (~z));
    }
    template <typename X,typename N>
    inline auto Rotate_Left(X x,N n) -> decltype((x<<n)|(x>>(32-n)))
    {
        return (x << n) | (x >> (32-n));
    }
    template <typename T,typename X,typename S,typename AC>
    inline void FF(T &a,T b,T c,T d,X x,S s,AC ac)
    {
        a += F(b,c,d) + x + ac;
        a = Rotate_Left(a,s);
        a += b;
    }
    template <typename T,typename X,typename S,typename AC>
    inline void GG(T &a,T b,T c,T d,X x,S s,AC ac)
    {
        a += G(b,c,d) + x + ac;
        a = Rotate_Left(a,s);
        a += b;
    }
    template <typename T,typename X,typename S,typename AC>
    inline void HH(T &a,T b,T c,T d,X x,S s,AC ac)
    {
        a += H(b,c,d) + x + ac;
        a = Rotate_Left(a,s);
        a += b;
    }
    template <typename T,typename X,typename S,typename AC>
    inline void II(T &a,T b,T c,T d,X x,S s,AC ac)
    {
        a += I(b,c,d) + x + ac;
        a = Rotate_Left(a,s);
        a += b;
    }
    void update(const byte *input, size_t length)
    {
        ulong i,index,partlen;
        finished = false;
        index = (ulong)((Count[0] >> 3) & 0x3f);
        if((Count[0] += ((ulong)length << 3)) < ((ulong)length << 3))
        {
            Count[1]++;
        }
        Count[1] += ((ulong)length >> 29);
        partlen = 64 - index;
        if(length >= partlen)
        {
            memcpy(&Buffer[index],input,partlen);
            transform(Buffer.data());
            for(i = partlen ;i+63 < length;i += 64)
            {
                transform(&input[i]);
            }
            index = 0;
        }
        else
        {
            i = 0;
        }
        memcpy(&Buffer[index],&input,length - i);
    }
    void final()
    {
        std::array<byte,8> bits;
        std::array<ulong,4> oldState;
        std::array<ulong,2> oldCount;
        ulong index, padLen;
        oldCount = Count;
        oldState = State;
        encode(Count.data(), bits.data(), 8);
        index = (ulong)((Count[0] >> 3) & 0x3f);
        padLen = (index < 56) ? (56 - index) : (120 - index);
        update(Padding.data(), padLen);
        update(bits.data(), 8);
        encode(State.data(), Digest.data(), 16);
        Count = oldCount;
        State = oldState;
    }
    void transform(const byte block[64])
    {
        ulong a = State[0], b = State[1], c = State[2], d = State[3], x[16];
	    decode(block, x, 64);
        FF(a, b, c, d, x[0], S11, 0xd76aa478); /* 1 */
        FF(d, a, b, c, x[1], S12, 0xe8c7b756); /* 2 */
        FF(c, d, a, b, x[2], S13, 0x242070db); /* 3 */
        FF(b, c, d, a, x[3], S14, 0xc1bdceee); /* 4 */
        FF(a, b, c, d, x[4], S11, 0xf57c0faf); /* 5 */
        FF(d, a, b, c, x[5], S12, 0x4787c62a); /* 6 */
        FF(c, d, a, b, x[6], S13, 0xa8304613); /* 7 */
        FF(b, c, d, a, x[7], S14, 0xfd469501); /* 8 */
        FF(a, b, c, d, x[8], S11, 0x698098d8); /* 9 */
        FF(d, a, b, c, x[9], S12, 0x8b44f7af); /* 10 */
        FF(c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
        FF(b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
        FF(a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
        FF(d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
        FF(c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
        FF(b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

        GG(a, b, c, d, x[1], S21, 0xf61e2562); /* 17 */
        GG(d, a, b, c, x[6], S22, 0xc040b340); /* 18 */
        GG(c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
        GG(b, c, d, a, x[0], S24, 0xe9b6c7aa); /* 20 */
        GG(a, b, c, d, x[5], S21, 0xd62f105d); /* 21 */
        GG(d, a, b, c, x[10], S22, 0x2441453); /* 22 */
        GG(c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
        GG(b, c, d, a, x[4], S24, 0xe7d3fbc8); /* 24 */
        GG(a, b, c, d, x[9], S21, 0x21e1cde6); /* 25 */
        GG(d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
        GG(c, d, a, b, x[3], S23, 0xf4d50d87); /* 27 */
        GG(b, c, d, a, x[8], S24, 0x455a14ed); /* 28 */
        GG(a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
        GG(d, a, b, c, x[2], S22, 0xfcefa3f8); /* 30 */
        GG(c, d, a, b, x[7], S23, 0x676f02d9); /* 31 */
        GG(b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

        HH(a, b, c, d, x[5], S31, 0xfffa3942); /* 33 */
        HH(d, a, b, c, x[8], S32, 0x8771f681); /* 34 */
        HH(c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
        HH(b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
        HH(a, b, c, d, x[1], S31, 0xa4beea44); /* 37 */
        HH(d, a, b, c, x[4], S32, 0x4bdecfa9); /* 38 */
        HH(c, d, a, b, x[7], S33, 0xf6bb4b60); /* 39 */
        HH(b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
        HH(a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
        HH(d, a, b, c, x[0], S32, 0xeaa127fa); /* 42 */
        HH(c, d, a, b, x[3], S33, 0xd4ef3085); /* 43 */
        HH(b, c, d, a, x[6], S34, 0x4881d05); /* 44 */
        HH(a, b, c, d, x[9], S31, 0xd9d4d039); /* 45 */
        HH(d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
        HH(c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
        HH(b, c, d, a, x[2], S34, 0xc4ac5665); /* 48 */
       
        II(a, b, c, d, x[0], S41, 0xf4292244); /* 49 */
        II(d, a, b, c, x[7], S42, 0x432aff97); /* 50 */
        II(c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
        II(b, c, d, a, x[5], S44, 0xfc93a039); /* 52 */
        II(a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
        II(d, a, b, c, x[3], S42, 0x8f0ccc92); /* 54 */
        II(c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
        II(b, c, d, a, x[1], S44, 0x85845dd1); /* 56 */
        II(a, b, c, d, x[8], S41, 0x6fa87e4f); /* 57 */
        II(d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
        II(c, d, a, b, x[6], S43, 0xa3014314); /* 59 */
        II(b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
        II(a, b, c, d, x[4], S41, 0xf7537e82); /* 61 */
        II(d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
        II(c, d, a, b, x[2], S43, 0x2ad7d2bb); /* 63 */
        II(b, c, d, a, x[9], S44, 0xeb86d391); /* 64 */

        State[0] += a;
        State[1] += b;
        State[2] += c;
        State[3] += d;
    }
    void encode(const ulong *input,byte *output,size_t nLength)
    {
        for (size_t i = 0, j = 0; j<nLength; i++, j += 4) 
        {
            output[j] = (byte)(input[i] & 0xff);
            output[j + 1] = (byte)((input[i] >> 8) & 0xff);
            output[j + 2] = (byte)((input[i] >> 16) & 0xff);
            output[j + 3] = (byte)((input[i] >> 24) & 0xff);
        }
    }
    void decode(const byte* input,ulong *output,size_t nLength)
    {
        for (size_t i = 0, j = 0; j<nLength; i++, j += 4) 
		output[i] = ((ulong)input[j]) | (((ulong)input[j + 1]) << 8) | (((ulong)input[j + 2]) << 16) | (((ulong)input[j + 3]) << 24);
    }
    std::string bytes_to_hexstring(const byte* input,size_t nLength)
    {
        std::string str;
        str.reserve(nLength << 1);
        for (size_t i = 0; i < nLength; i++) 
        {
            int t = input[i];
            int a = t / 16;
            int b = t % 16;
            str.append(1, Hex[a]);
            str.append(1, Hex[b]);
        }
        return str;
    }
    MD5(const MD5&)
    {

    }
    MD5& operator=(const MD5&);
};

const std::array<byte,64> MD5::Padding {0x80};
const std::array<char,16> MD5::Hex 
    {
        '0','1','2','3',
        '4','5','6','7',
        '8','9','a','b',
        'c','d','e','f'
    };
const size_t MD5::BufSize = 1024;
#endif
