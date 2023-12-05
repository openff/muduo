#ifndef EVENTLOOPTHREAD_H
#define EVENTLOOPTHREAD_H
#include"noncopyable.h"
#include"Thread.h"

#include<functional>
#include<mutex>
#include<condition_variable>
#include<string>
class EventLoop;

/**
 * one pair loop thread
 * 
*/
class EventLoopThread:noncopyable
{

public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;
    
    EventLoop* startLoop();
    
    EventLoopThread(const ThreadInitCallback& cb =ThreadInitCallback(),const std::string &name=std::string());
    ~EventLoopThread();

private:
    /* data */
    void threadFunc();


    EventLoop* loop_{nullptr};
    bool exiting_{false};
    Thread thread_;
    std::mutex mtx_{};
    std::condition_variable cond_{};
    ThreadInitCallback callback_;
};




#endif