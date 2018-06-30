#include "thread_pool.h"

Client* pHead = NULL;
Client* pEnd = NULL;
int client_num = 0;

void client_delete(int clientfd)
{
    pthread_mutex_lock(&alock);
    Client* pPre = pHead;
    Client* pTemp = pHead->pNext;
    if(pHead == NULL)
    {
        return;
    }
    if(pHead->clientfd == clientfd)
    {
        pHead = pHead->pNext;
        free(pPre);
    }
    while(pTemp)
    {
        if(pTemp->clientfd == clientfd)
        {
            pPre->pNext = pTemp->pNext;
            if(pTemp == pEnd)
            {
                pEnd = pPre;
            }
            free(pTemp);
        }
        pPre = pPre->pNext;
        pTemp = pTemp->pNext;
    }
    client_num--;
    pthread_mutex_unlock(&alock);
}

void client_append(Client** client)
{
    printf("append\n");
    pthread_mutex_lock(&alock);
    if(client == NULL)
    {
        return;
    }
    if(pHead == NULL)
    {
        printf("first\n");
        pHead = *client;
        pEnd = *client; 
    }
    else
    {
        (*client)->pNext = pHead;
        pHead = *client;
    }
    client_num++;
    pthread_mutex_unlock(&alock);
    client_show();
}

void client_show()
{
    pthread_mutex_lock(&alock);
    printf("show\n");
    Client *pTemp = pHead;
    while(pTemp)
    {
        printf("%d ",pTemp->type);
        pTemp = pTemp->pNext;
    }
    printf("\n");
    pthread_mutex_unlock(&alock);
}

Client* find_client(int type)
{
    pthread_mutex_lock(&alock);
    printf("Find client %d",type);
    Client* pTemp = pHead;
    while(pTemp)
    {
        if(pTemp->type == type)
        {
            pthread_mutex_unlock(&alock);
            return pTemp;
        }
        pTemp = pTemp->pNext;
    }
    printf("Not find client ...\n");
    pthread_mutex_unlock(&alock);
    return NULL;
}

Client* find_next_client(Client* client)
{
    switch(client->type)
    {
        case USER_URL:
            {
               return find_client(USER_HTML);
            }
           break; 
        case PRE_URL:
            {
                return find_client(PRE_HTML);
            }
           break; 
        case USER_HTML:
            {
                return find_client(PRE_URL);
            }
           break; 
        case PRE_HTML:
            {
                return find_client(USER_URL);
            }
           break; 
    }
    printf("Find Client Error...\n");
    return NULL;
}

pool_t * Pool_Create(int thread_min , int thread_max , int queue_max)
{
    pool_t * pool = NULL;
    int i=0;
    if((pool = (pool_t * )malloc(sizeof(pool_t)))==NULL)
    {
        perror("Pool_Create() pool Malloc:");
        return NULL;
    }
    pool->pool_min = thread_min;
    pool->pool_max = thread_max;
    pool->alive = 0;
    pool->busy = 0;
    pool->queue_front = 0;
    pool->queue_rear = 0;
    pool->queue_max = queue_max;
    pool->queue_size = 0;
    pool->pool_shutDown = TRUE;
    if((pool->threads = (pthread_t *)malloc(thread_max * sizeof(pthread_t)))==NULL)
    {
        perror("Pool_Create() threads Malloc:");
        return NULL;
    }
    memset(pool->threads,0,thread_max * sizeof(pthread_t));
    if((pool->task_queue = (task_t *)malloc(queue_max * sizeof(task_t)))==NULL)
    {
        perror("Pool_Create() task_queue Malloc:");
        return NULL;
    }
    if(pthread_mutex_init(&pool->pool_lock,NULL)!=0||pthread_mutex_init(&pool->arg_lock,NULL)!=0||pthread_cond_init(&pool->not_full,NULL)!=0||pthread_cond_init(&pool->not_empty,NULL)!=0)
    {
        perror("Pool_Create() init Mutex or Cond:");
        return NULL;
    }
    for(i;i<CREATE_DES;i++)
    {
        pool->alive++;
        pthread_create(&pool->threads[i],NULL,Pool_Def_Task,(void *)pool);
    }
    pthread_create(&pool->Manager_tid,NULL,Pool_Mana_Task,(void *)pool);
    return pool;
}
int Pool_Add_TaskQueue(pool_t * pool,void * (*Work)(void*arg),void*arg)
{
    pthread_mutex_lock(&(pool->pool_lock));
    while(pool->queue_size == pool->queue_max && pool->pool_shutDown==TRUE)
    {
        pthread_cond_wait(&pool->not_full,&pool->pool_lock);
    }
    if(pool->pool_shutDown == FALSE){
        pthread_mutex_unlock(&pool->pool_lock);
    }
    if(pool->task_queue[pool->queue_rear].arg != NULL)
    {
        free(pool->task_queue[pool->queue_rear].arg);
        pool->task_queue[pool->queue_rear].arg = NULL;
    }
    pool->task_queue[pool->queue_rear].task = Work;
    pool->task_queue[pool->queue_rear].arg = arg;
    pool->queue_rear = (pool->queue_rear+1)%pool->queue_max;
    pool->queue_size++;
    pthread_cond_signal(&pool->not_empty);
    pthread_mutex_unlock(&pool->pool_lock);
    return 0;
}
void * Pool_Def_Task(void * p)
{
    pool_t * pool = (pool_t *)p;
    task_t tmptask;
    printf("thread_id:%x waiting....\n",(unsigned int)pthread_self());
    while(TRUE)
    {
        pthread_mutex_lock(&(pool->pool_lock));
        while(pool->queue_size == 0 && pool->pool_shutDown == TRUE )
        {
            pthread_cond_wait(&pool->not_empty,&pool->pool_lock);
            if(pool->wait > 0 && pool->pool_min < pool->alive)
            {
                printf("thread_id:%x exited\n",(unsigned int)pthread_self());
                pool->wait--;
                pool->alive--;
                pthread_mutex_unlock(&(pool->pool_lock));
                pthread_exit(NULL);
            }
        }
        if(pool->pool_shutDown == FALSE)
        {
            pthread_mutex_unlock(&(pool->pool_lock));
            pthread_exit(NULL);
        }
        tmptask.task = pool->task_queue[pool->queue_front].task;
        tmptask.arg = pool->task_queue[pool->queue_front].arg;
        pool->queue_front = (pool->queue_front + 1)% pool->queue_max;
        pool->queue_size--;
        pthread_cond_signal(&(pool->not_full));
        pool->busy++;
        pthread_mutex_unlock(&(pool->pool_lock));
        (*(tmptask.task))(tmptask.arg);
        pthread_mutex_lock(&(pool->pool_lock));
        pool->busy--;
        pthread_mutex_unlock(&(pool->pool_lock));
    }
    pthread_exit(NULL);
}
void * Pool_Mana_Task(void * p)
{
    pool_t * pool = (pool_t *)p;
    int alive,busy,size;
    int i;
    printf("Manager_thread Start....\n");
    while(pool->pool_shutDown)
    {
        pthread_mutex_lock(&(pool->pool_lock));
        alive = pool->alive;
        busy = pool->busy;
        size = pool->queue_size;
        pthread_mutex_unlock(&(pool->pool_lock));

        if((size > (alive - busy) || ((float)busy / alive) * 100 >= (float)50) && alive < pool->pool_max)
        {
            pthread_mutex_lock(&(pool->arg_lock));
            int add=0;
            for(i=0;i<pool->pool_max && add < CREATE_DES && alive < pool->pool_max;i++)
            {
                if(pool->threads[i]==0||!if_alive_thread(pool->threads[i])){
                    pthread_create(&pool->threads[i],NULL,Pool_Def_Task,(void *)pool);
                    printf("Manager Create pthread_tid:0x%x\n",(unsigned int)pool->threads[i]);
                    add++;
                    pool->alive++;
                }
            }	
            pthread_mutex_unlock(&(pool->arg_lock));
        }
        if((busy * 2) < alive && alive > pool->pool_min)
        {
            pthread_mutex_lock(&(pool->pool_lock));
            pool->wait = CREATE_DES;
            pthread_mutex_unlock(&(pool->pool_lock));
            for(i=0;i<CREATE_DES;i++)
                pthread_cond_signal(&(pool->not_empty));
        }
        printf("Manager_thread output info:\n");
        printf("存活线程:%d   忙线程:%d   闲线程:%d  忙/存活:%.2f%%  存活/所有:%.2f%%\n",alive,busy,(alive-busy),((float)pool->busy/pool->alive)*100,((float)pool->alive/pool->pool_max)*100);
        printf("\n");
        sleep(_TIMEOUT);
    }
    pthread_exit((void * )0);
}
int if_alive_thread(pthread_t tid)
{
    if((pthread_kill(tid,0))!=0)
        if(errno == ESRCH)
            return FALSE;
    return TRUE;
}
int init_socket(void)
{
    struct sockaddr_in serveraddr;
    int socketfd;
    bzero(&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(_PORT);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    socketfd = socket(AF_INET,SOCK_STREAM,0);
    if((bind(socketfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr)))==-1)
    {
        perror("Bind Error:");
        return -1;
    }
    listen(socketfd,_LISTEN);
    return socketfd;
}

void * server_work(void * arg)
{
    int socketfd = (long int)arg;
    int clientfd,clientsize,len;
    Client* client = (Client*)malloc(sizeof(client));
    struct sockaddr_in clientaddr;
    char buf[_BUF_SIZE];
    char ipstr[_IP_SIZE];
    clientsize = sizeof(clientaddr);
    pthread_mutex_lock(&alock);
    printf("tid:%x  Accepting....\n",(unsigned int)pthread_self());
    clientfd = accept(socketfd,(struct sockaddr *)&clientaddr,&clientsize);
    client->clientfd = clientfd;
    strcpy(client->HOST,inet_ntop(AF_INET,&clientaddr.sin_addr.s_addr,ipstr,_IP_SIZE));
    client->PORT = ntohs(clientaddr.sin_port);
    client->pNext = NULL;
    pthread_mutex_unlock(&alock);
    printf("tid:%x  ip:%s  port:%d\n",(unsigned int)pthread_self(),inet_ntop(AF_INET,&clientaddr.sin_addr.s_addr,ipstr,_IP_SIZE),ntohs(clientaddr.sin_port));
    int flag = 0;
    while((len = read(client->clientfd,buf,sizeof(buf)))>0)
    {
        printf("Read msg from client");
        pthread_mutex_lock(&alock);
        client->type = atoi(buf);
        pthread_mutex_unlock(&alock);
        if(client->type >=100 && client->type <= 103)
            break;
        bzero(buf,sizeof(buf));
    }
    client_append(&client); 
    printf("Get datas from client:%d,IP:%s,PORT:%d\n",client->type,client->HOST,client->PORT);
    while(1)
    {
        char wait[5] = "WAIT";
        write(client->clientfd,wait,sizeof(wait));
        printf("Wait for client logging ...client:%d\n",client_num);
        if(client_num >= 4)
        {
            char ok[3] = "OK";
            printf("Send OK msg ...\n");
            write(client->clientfd,ok,sizeof(ok));
            break;
        }
        sleep(2);
    }
    Client* next_client = find_next_client(client);
    printf("Get the next client:%d,IP:%s,PORT:%d\n",next_client->type,next_client->HOST,next_client->PORT);
    while((len = read(client->clientfd,buf,sizeof(buf)))>0)
    {
        printf("Read msg from client:%d\n,msg:%s\n",client->type,buf);
        write(next_client->clientfd,buf,len);
        bzero(buf,sizeof(buf));
    }
    if(len == 0){
        close(clientfd);
        client_delete(clientfd);
    }
    return NULL;
}


void Daemon(void)
{
    pid_t pid;
    int fd;
    pid = fork();
    if(pid > 0)
    {
        exit(0);
    }
    else if(pid == 0)
    {
        setsid();
        chdir("/");
        umask(0);
        fd = open("/tmp/thread_pool.log",O_RDWR|O_CREAT,0664);
            perror("dup2 error:");
        printf("hello world\n");
    }
    else
    {
        perror("fork error:");
    }
}

int main(void)
{
    long int sockfd;
    int epfd;
    struct epoll_event evt;
    struct epoll_event evearr[1];
    int ready;
    pool_t * pool; 
    sockfd = init_socket();
    if (sockfd == -1)
        return 0;
    Daemon();
    epfd = epoll_create(10);
    evt.data.fd = sockfd;
    evt.events = EPOLLIN;
    evt.events |= EPOLLET;
    epoll_ctl(epfd,EPOLL_CTL_ADD,sockfd,&evt);
    pool = Pool_Create(10,300,1000);
    while(1)
    {
        ready = epoll_wait(epfd,evearr,1,-1);
        if(evearr[0].data.fd == sockfd)
            Pool_Add_TaskQueue(pool,server_work,(void*)sockfd);
    }
}
