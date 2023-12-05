
#include"EventLoopThread.h"
#include "EventLoopThreadPool.h"


#include<memory>

EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseloop, const std::string &name)
        :baseLoop_(baseloop)
        ,name_(name)
{

}
EventLoopThreadPool::~EventLoopThreadPool(){

}

void EventLoopThreadPool::start(const ThreadInitCallback &cb){
    started_ = true;
    /**多线程模式 创建其他loop(work loop)*/
    for (size_t i = 0; i < numThreads_; ++i)
    {
        /**用户设置了多个线程模式*/
        std::string buf(name_.data()+std::to_string(i));
        std::unique_ptr<EventLoopThread> t = std::make_unique<EventLoopThread>(cb,buf);
        threads_.emplace_back(std::move(t));
        /**底层返回一个loop 这个loop是栈区的 */
        loops_.push_back(threads_.back()->startLoop());
       
    }

    //调用端未设置线程数量
    if (numThreads_ ==  0 && cb)
    {
        cb(baseLoop_);
    }
    
    
}
EventLoop *EventLoopThreadPool::getNextLoop(){
    /**当不是多线程模式，则主要使用baseloop工作*/
    EventLoop* loop = baseLoop_;
    if (!loops_.empty())
    {
        /* 通过轮询的方式取loop工作*/
        loop = loops_[next_];
        ++next_;
        if (static_cast<size_t>(next_) >= loops_.size())
        {
            /**重置*/
            next_ = 0;
        }
    }
    return loop;
}
std::vector<EventLoop *> EventLoopThreadPool::getAllLoop(){
    if (loops_.empty())
    {
        return std::vector<EventLoop*>(1,baseLoop_);
    }else{
        return loops_;
    }
    
}
