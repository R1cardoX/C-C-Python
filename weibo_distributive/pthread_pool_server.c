#include "../include/thread_pool.h"

int clientnum = 0;
int client[3] = {0};


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
    }
    listen(socketfd,_LISTEN);
    return socketfd;
}
int clientfd[4];

void * server_work(void * arg)
{
    int socketfd = (long int)arg;
    int client,clientsize,len;
    struct sockaddr_in clientaddr;
    char buf[_BUF_SIZE];
    char ipstr[_IP_SIZE];
    clientsize = sizeof(clientaddr);
    pthread_mutex_lock(&alock);
    printf("tid:%x  Accepting....\n",(unsigned int)pthread_self());
    client = accept(socketfd,(struct sockaddr *)&clientaddr,&clientsize);
    pthread_mutex_unlock(&alock);
    printf("tid:%x  ip:%s  port:%d\n",(unsigned int)pthread_self(),inet_ntop(AF_INET,&clientaddr.sin_addr.s_addr,ipstr,_IP_SIZE),ntohs(clientaddr.sin_port));
    int flag = 0;
    while((len = read(client,buf,sizeof(buf)))>0)
    {
        int j = 0;
        if(flag == 0)
        {
            //{'type':'url1/url2/html1/html2',......}
            int i = 9;
            char type[6];
            while(buf[i] == '\'')
            {
               type[i-9] = buf[i];
            }
            if (strcmp(type,"url1") == 0)
            {
                clientfd[0] = client;  
                j = 1;
            }
            else if(strcmp(type,"url2") == 0)
            {
                clientfd[1] = client;
                j = 2;
            }
            else if(strcmp(type,"html1") == 0)
            {
                clientfd[2] = client;
                j = 3;
            }
            else if(strcmp(type,"html2") == 0)
            {
                clientfd[3] = client;
                j = 0;
            }
        } 
        write(clientfd[j],buf,len);
        bzero(buf,sizeof(buf));
    }
    if(len == 0){
        close(client);
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
//	    close(STDIN_FILENO);
//		close(STDERR_FILENO);
        fd = open("/tmp/thread_pool.log",O_RDWR|O_CREAT,0664);
//		if((dup2(fd,STDOUT_FILENO))==-1)
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
    /*epoll完成任务投递*/
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
