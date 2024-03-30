#ifndef THREAD_POOL_H
#define THREAD_POOL_H
/******************
 * C++线程池  三个要素
 * 1、任务队列
 * 2、工作线程
 * 3、管理函数
 * 
 * *****************/
#include <iostream>
#include <memory>
#include <queue>
#include <vector>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>

class ThreadPool{
private:
    unsigned int max_num_;
    std::vector<std::thread>  threads_;
    std::queue<std::function<void()>> tasks_;
    std::mutex mutex_;
    std::condition_variable cond_;
    bool stop_ = false;

public:
    ThreadPool(const unsigned int &max_num):max_num_(max_num){
        for(int i = 0;i < max_num; i++){
            threads_.emplace_back([this](){  
                while(true){
                    std::unique_lock<std::mutex> lock(mutex_);
                    cond_.wait(lock, [this](){return !tasks_.empty() || stop_;});//如果lambda = false，则阻塞，直到收到信号，然后继续判断lambda，等于false继续阻塞，继续等待信号。。。
                    if(stop_ && tasks_.empty()){return;}
                    std::function<void()> task(std::move(tasks_.front()));
                    tasks_.pop();
                    lock.unlock();
                    task();
                }
            });
        }
    };
    ~ThreadPool(){
        {
            std::unique_lock<std::mutex> lock(mutex_);
            stop_ = true;
        }
        cond_.notify_all(); //Remind all thread that is waitting to return
        for(auto &thread:threads_){
            thread.join(); //Wait all thread that doing finish final task
        }
    }

    //add task, and start it
    template<class F,class... Args>
    void AddTask(F &&f, Args&&... args){
        std::function<void()>task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        {
            std::unique_lock<std::mutex> lock(mutex_);
            tasks_.emplace(std::move(task));
        }
        cond_.notify_all();
    }
};
#endif