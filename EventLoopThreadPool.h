#ifndef EventLoopThreadPool_H
#define EventLoopThreadPool_H

#include "noncopyable.h"

#include "EventLoopThread.h"

class EventLoopThreadPool : noncopyable
{
public:
    using ThreadInitCallback = std::function<void(EventLoop*)> ;

    EventLoopThreadPool(EventLoop* baseloop,const std::string &name);
    ~EventLoopThreadPool();

    void setThreadNum(int numThreads){numThreads_ = numThreads;}
    void start(const ThreadInitCallback &cb = ThreadInitCallback());
    EventLoop* getNextLoop();
    std::vector<EventLoop*> getAllLoop();

    bool started()const{return started_;}
    const std::string& name()const{return name_;}
private:
    /* data */
    EventLoop *baseLoop_{nullptr};
    std::string name_{};
    bool started_{false};
    int numThreads_{0};
    int next_{0};
    std::vector<std::unique_ptr<EventLoopThread>> threads_;
    std::vector<EventLoop *> loops_;
};

#endif