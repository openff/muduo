#include "EventLoopThread.h"
#include"EventLoop.h"
EventLoop *EventLoopThread::startLoop(){
    //底层调用新线程开始执行
    thread_.start();
    EventLoop* loop = nullptr;
    {
        std::unique_lock<std::mutex> lock(mtx_);
        while (loop ==  nullptr)
        {
            cond_.wait(lock);
        }
        loop = loop_;
    }
    return loop;
}

EventLoopThread::EventLoopThread(const ThreadInitCallback &cb, const std::string &name)
    :thread_(std::bind(&EventLoopThread::threadFunc,this),name)
    ,callback_(cb)
{

}
EventLoopThread::~EventLoopThread(){
    exiting_ = true;
    if (loop_ != nullptr){
        loop_->quit();
        thread_.join();
    }
}
/**
 * 此函数是在底层Thread开辟子线程执行的函数func
*/
void EventLoopThread::threadFunc(){
    //实例 一个loop 一个线程
    EventLoop loop;
    if (callback_){
        callback_(&loop);
    }

    {
       std::unique_lock<std::mutex> lock(mtx_);
       loop_ = &loop;
    }

    cond_.notify_one();
    //event loop 事件循环真正开始
    loop.loop();

    std::unique_lock<std::mutex> lock(mtx_);
    loop_ = nullptr;
}