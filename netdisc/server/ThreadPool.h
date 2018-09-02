#pragma once
#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <memory>
#include <iostream>
#include <functional>
#include <condition_variable>
#include <atomic>
#include <type_traits>
#define CERR(x) std::cerr<<(#x)<<std::endl;
#define Debug(x) std::cerr<<(#x)<<" = "<<(x)<<std::endl;
#define showList(x) do{std::cerr<<"List->"<<(#x)<<":"<<std::endl; for(auto :x) std::cerr<<x<<" ";std::cerr<<std::endl;}while(0)
#define showVector(x) do{std::cerr<<"Vector->"<<(#x)<<":"<<std::endl; for(auto :x) std::cerr<<x<<" ";std::cerr<<std::endl;}while(0)
#define showQueue(x) do{std::cerr<<"Queue->"<<(#x)<<":"<<std::endl; for(auto :x) std::cerr<<x<<" ";std::cerr<<std::endl;}while(0)

static const std::size_t max_task_quque_size = 100000;
static const std::size_t max_thread_size = 30;

class ThreadPool
{
public:
    using Thread_ptr = std::shared_ptr<std::thread>;
    using Task_t = std::function<void()>;
private:
    std::vector<Thread_ptr> Thread_vec;
    std::condition_variable Task_put;
    std::condition_variable Task_get;
    std::mutex Mutex;
    std::queue<Task_t> Task_queue;
    std::atomic<bool> Is_stop_threadpool;
    std::once_flag Call_flag;
public:
    explicit ThreadPool():Is_stop_threadpool(false){}
    ~ThreadPool()
    {
        stop();
    }
    bool Init_Thead(int num = 10)
    {
        if(num<=0)
        {
            std::cerr<<"Number of threads is Wrong"<<std::endl;
            return false;
        }
        for(int i = 0; i < num; i++)
        {
            Thread_ptr pWorkThread = std::make_shared<std::thread>(std::bind(&ThreadPool::Run_Task,this));
            Thread_vec.emplace_back(pWorkThread);
        }
        std::cerr<<"Init TheadPool Success ...."<<std::endl;
        return true;
    }
    template<typename Function,typename... Args>
    void Push_Task(const Function& func,Args... args)
    {
        if(!Is_stop_threadpool)
        {
            Task_t task = [&func,args...]{return func(args...);};
            Add_Task_Impl(task);
        }
    }
    template<typename Function,typename... Args>
    typename std::enable_if<std::is_class<Function>::value>::type Push_Task(Function &func,Args... args)
    {
        if(!Is_stop_threadpool)
        {
            Task_t task = [&func,args...]{return func(args...);};
            Add_Task_Impl(task);
        }
    }
    template<typename Function,typename Self,typename... Args>
    void Push_Task(const Function& func,Self* self,Args... args)
    {
        if(!Is_stop_threadpool)
        {
            Task_t task = [&func,&self,args...]{return (*self.*func)(args...);};
            Add_Task_Impl(task);
        }
    }
    void stop()
    {
        std::call_once(Call_flag,[this]{Terminate_All();});
    }
private:
    void Add_Task_Impl(const Task_t& task)
    {
        CERR(task);
        std::unique_lock<std::mutex> Lock(Mutex);
        while(Task_queue.size() == max_task_quque_size && !Is_stop_threadpool)
        {
            Task_put.wait(Lock);
        }
        Task_queue.emplace(std::move(task));
        Task_get.notify_one();
    }
    void Terminate_All()
    {
        Is_stop_threadpool = true;
        Task_get.notify_all();
        for(auto &iter : Thread_vec)
        {
            if(iter != nullptr)
            {
                if(iter->joinable())
                {
                    iter->join();
                }
            }
        }
        Thread_vec.clear();
        Clean_Task_Queue();
    }
    void Run_Task()
    {
        while(true)
        {
            Task_t task = nullptr;
            std::unique_lock<std::mutex> Lock(Mutex);
            while(Task_queue.empty() && !Is_stop_threadpool)
            {
                Task_get.wait(Lock);
            }
            if(Is_stop_threadpool)
            {
                break;
            }
            if(!Task_queue.empty())
            {
                task = std::move(Task_queue.front());
                Task_queue.pop();
            }
            if(task != nullptr)
            {
                task();
                Task_put.notify_one();
            }
        }
    }
    void Clean_Task_Queue()
    {
        std::lock_guard<std::mutex> Lock(Mutex);
        while(!Task_queue.empty())
        {
            Task_queue.pop();
        }
    }
};
#endif
