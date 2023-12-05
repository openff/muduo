#ifndef THREAD_H
#define THREAD_H

#include"noncopyable.h"
#include<functional>
#include<cstring>
#include<atomic>
#include<mutex>
#include<thread>
#include<memory>

class Thread:noncopyable
{
public:
    using ThreadFunc = std::function<void()>;
    explicit Thread(ThreadFunc ,const std::string &name = std::string());
    ~Thread();

    void start();
    void join();

    bool started()const { return started_; }
    pid_t tid()const { return tid_;}
    const std::string & name()const{ return name_; }
    static int numCreated() { return numCreated_;}
private:
    void setDefaultName();
    /* data */
    bool started_{false};
    bool joined_{false};
    std::shared_ptr<std::thread> thread_;
    pid_t tid_{0};
    ThreadFunc func_;
    std::string name_;
  
    static std::atomic_int32_t numCreated_;
};




#endif