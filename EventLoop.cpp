#include"Poller.h"
#include "EventLoop.h"
#include "Logger.h"
#include "DefaultPoller.h"
#include "Channel.h"

#include <sys/eventfd.h>
#include <error.h>
#include <memory>
#include <thread>
#include<vector>


// 防止一个线程创建多个loop
__thread EventLoop *t_loopInThread = nullptr;

// 设置循环超时时间
const int kPollTime = 1000;

int createEventfd()
{
    /**函数原理
     * eventfd 会返回一个event fd 可以任意写入数据并对计数器+n 当读数据时会把计数器清0
     *  write(fd ,&u ,8)  假设当前u的值是 1 该函数表示 写入fd 值u 字节8(固定)
     *  write(fd ,&u ,8)  假设当前u的值是 2
     *  write(fd ,&u ,8)  假设当前u的值是 3
     *  read(fd ,&u ,8)   前面写了三次 此处读取 会得到u = 6 （3+2+1）并清空fd的计数器
     *
     * 具体函数实现是eventfd_write  eventfd_read
     * 该fd不会走sock缓冲区，直接通过内核投递时间 性能很高
     */
    int evfd = eventfd(0, 0);
    if (evfd < 0)
    {
        LOG_FATAL("eventfd fun error: %d", errno);
    }
    return evfd;
}
EventLoop::EventLoop() : looping_(false), quit_(false), callingPendingFunctor_(false), threadId_(CurrentThread::tid()), poller_(std::move(Poller::newDefaultPoller(this))), weakupFd_(createEventfd()), weakupChannel_(std::make_unique<Channel>(this, this->weakupFd_)), currentActiveChannel_(nullptr)
{
    LOG_DEBUG("EventLoop() create %p  in thread ID: %d", this, threadId_);
    if (t_loopInThread)
        LOG_FATAL("EventLoop() error loop not thread");

    t_loopInThread = this;
    // 设置event loop 事件类型以及回调操作 main loop 与 work loop 间的通信
    weakupChannel_->setReadEventCallback(std::bind(&EventLoop::handleRead, this));
    // 每个event loop 都将监听weakupchannel 的EPOLLIN事件
    weakupChannel_->enableReading();
}
EventLoop::~EventLoop()
{
    weakupChannel_->disableAll();
    weakupChannel_->remove();
    close(weakupFd_);
    t_loopInThread = nullptr;
}
void EventLoop::handleRead()
{
    uint64_t value = 0;
    ssize_t ret = eventfd_read(this->weakupFd_, &value);
    if (ret < 0)
    {
        LOG_ERROR("handleRead eventfd_read  error");
    }
    else if (ret == sizeof(uint64_t))
    {
        // 计数值读取成功
        // 处理value的值
    }
}

void EventLoop::loop()
{
    looping_ = true;
    quit_ = false;

    LOG_INFO("EventLoop start :%p", this);
    while (!quit_)
    {
        activeChannels_.clear();
        // 通过调度器收集有事件的channel
        /**监听两类fd  client channelfd 处理事件的fd  mainloop 与 workloop 通信的fd*/
        pollerRetTime_ = poller_->poll(kPollTime, &activeChannels_);
        for (Channel *channel : activeChannels_)
        {
            /* 处理有事件的channel*/
            channel->handleEvent(pollerRetTime_);
        }
        /**执行完event loop 事件循环所需要处理的回调操作
         *
         * IO线程 mainloop  acceptfd  <-- workloop channel
         * mainloop 事先注册一个回调cb(由workloop执行) weakup workloop后执行下面方法(执行mainloo所注册的cb操作)
         */
        doPendingFunctor();
    }
}
/**
 * 退出事件循环
 * 情况1：在自己线程调用quit
 * 情况2：在其他线程调用quit 则唤醒其他线程执行。。。来执行quit(帮其他线程event loop 解while(!quit_))
 */
void EventLoop::quit()
{
    quit_ = true;
    if (!isInLoopThread())
    {
        weakup();
    }
}

// 在当前线程执行cb
void EventLoop::runInLoop(Functor cb)
{
    if (isInLoopThread())
    {
        cb();
    }
    else
    {
        queueInLoop(cb);
    }
}
// 投递给任务队列 唤醒loop所在线程 暂不执行cb
void EventLoop::queueInLoop(Functor cb)
{
    {
        std::unique_lock<std::mutex> lock(mtx_);
        pendingFunctors_.emplace_back(cb);
    }

    /**
     * 非当前线程 or 当前线程正在清理回调函数集合时 则唤醒
     */
    if (!isInLoopThread() || callingPendingFunctor_)
    {
        weakup();
    }
}

// 唤醒loop所在线程
void EventLoop::weakup()
{
    uint64_t num = 0;
    int ret = write(weakupFd_, &num, sizeof num);
    if (ret != sizeof num)
    {
        LOG_ERROR("wakeup write error\n");
    }
}

// EventLoop -> poller
void EventLoop::updateChannel(Channel *channel)
{
    poller_->updateChannel(channel);
}
void EventLoop::removeChannel(Channel *channel)
{
    poller_->removeChannel(channel);
}
bool EventLoop::hasChannel(Channel *target) const
{
    return poller_->hasChannel(target);
}

// 执行回调
/**
 * 用空间换时间的概念，把类内swap到局部，当运行回调时类内依然可以接收cb。
 * 如果使用类内的一个一个取，则需要加锁。这导致锁的粒度很高
*/
void EventLoop::doPendingFunctor()
{
    std::vector<Functor> func;
    callingPendingFunctor_ = true;
    {
        std::unique_lock<std::mutex> lock(mtx_); 
        func.swap(pendingFunctors_);
    }

    for (const Functor &f : func)
    {
        f();
    }

    callingPendingFunctor_ =false;
    
}

