#include "Thread.h"
#include"CurrentThread.h"

#include<semaphore.h>
std::atomic_int32_t Thread::numCreated_(0);
Thread::Thread(ThreadFunc func, const std::string &name) 
        :func_(std::move(func))
        ,name_(name)    
{
    setDefaultName();
}
Thread::~Thread() {
    if (started_ && !joined_){
        thread_->detach();
    }
}

void Thread::start() {
    started_ = true;
    //创建一个信号量
    sem_t sem;
    //初始化信号量 false不使用多进程 默认值0
    sem_init(&sem,false,0);

    //开启一个子线程执行fun makeshared 是不会执行函数的
    thread_ = std::make_shared<std::thread>(std::thread([&](){
        //设置线程id 此id 是linux 的id 非线程句柄id
        tid_ = CurrentThread::tid();
        //设置信号量+1
        sem_post(&sem);
        //执行函数
        this->func_();
    }));

    //等待子线程执行完tid设置后继续执行
    sem_wait(&sem);

}
void  Thread::join() {
    joined_ = true;
    thread_->join();
}

void Thread::setDefaultName()
{
    int num = ++numCreated_;
    if (name_.empty())
    {
        name_ += std::to_string(num);
    }
}
