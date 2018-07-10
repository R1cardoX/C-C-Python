#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#define FIFO_RECV "./fifo_recv"
#define FIFO_SEND "./fifo_send"
#define BUFSIZE 1024

int* next(char*,int);
int* KMP(char*,int,char*,int);
char* sensitive_replace(char*,int,FILE*);

int main()
{
    int pipe_recv_fd;
    int pipe_send_fd;
    FILE* fd;
    int res;
    char buf[BUFSIZE + 1];
    char* finally;
    memset(buf,0, sizeof(buf));

    if(access(FIFO_SEND, F_OK) == -1)
    {
        res = mkfifo(FIFO_SEND, 0777);
        if(res != 0)
    {
            exit(EXIT_FAILURE);
        }
    }
    if(access(FIFO_RECV, F_OK) == -1)
    {
        res = mkfifo(FIFO_RECV, 0777);
        if(res != 0)
        {
            exit(EXIT_FAILURE);
        }
    }
    pipe_recv_fd = open(FIFO_RECV,O_RDWR,0);
    pipe_send_fd = open(FIFO_SEND,O_RDWR,0);
    fd = fopen("badword","r");

    printf("Open Success ... \n");

    if (pipe_recv_fd != -1 && pipe_send_fd != -1)
    {
        printf("Begin  \n");
        do{
            res = read(pipe_recv_fd,buf,BUFSIZE);
            if(strcmp(buf,"quit") == 0)
                break;
            ftruncate(pipe_recv_fd,0);
            printf("%s\n",buf);
            finally = sensitive_replace(buf,res,fd);
            write(pipe_send_fd,finally,res);
        }while(res > 0);
        close(pipe_recv_fd);
        close(pipe_send_fd);
        printf("Exit1\n");
    }
    else
    {
        printf("Exit2\n");
        exit(EXIT_FAILURE);
    }
    printf("Exit3\n");
    return 0; 
}

char* sensitive_replace(char* original,int len,FILE* fd)
{
    char* sesitive = (char*)malloc(sizeof(char)*20);
    char* finally = (char*)malloc(sizeof(char)*len);
    char s;
    int *match = (int*)malloc(sizeof(int)*len);
    int i = 0;
    memset(sesitive,0,sizeof(char)*20);
    memset(match,-1,sizeof(int));
    finally = original;
    while(1)
    {
        s = fgetc(fd);
        if(s == ' ')
        {
            match = KMP(original,len,sesitive,i+1);
            int j = 0;
            while(match[j] != -1)
            {
                for(int c = j;c < i+j+1;c++)
                {
                    finally[j] = '*';
                }
            }
            memset(sesitive,0,sizeof(char)*20);
        }
        sesitive[i] = s;
        i++;            
    }
    return finally;
} 

int* next(char* parrton,int len)
{
    int *result = (int*)malloc(sizeof(int)*len);
    for(int i=0;i<len;i++)
    {
        int j = result[i-1];
        while(j&&parrton[i] != parrton[j])
            j = result[j-1];
        result[i] = parrton[i] == parrton[j] ? j+1:0;
    }
    return result;
}


int* KMP(char* str,int str_len,char* parrton,int parrton_len)
{
    int* arr = next(parrton,parrton_len);
    int* result = (int*)malloc(sizeof(int)*str_len);
    int p = 0;
    int j = 0;
    for(int i=0;i<str_len;i++)
    {
        while(p>0 && parrton[p] != str[i])
            p = arr[p-1];
        if(str[i] == parrton[p])
            p++;
        if(p == parrton_len)
            result[j] = i-p+1;
    }
    return result;
}

