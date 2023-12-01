#ifndef EVENTLOOP_H
#define EVENTLOOP_H

/**
 * 线程通信机制：
 * #include <sys/eventfd.h>
    int eventfd(unsigned int initval, int flags);
 * 通过内核通知来传递信息  使得线程之间通信
    优点：高性能        缺点：不可跨平台

*  #include <sys/socket.h>
    int socketpair(int domain, int type, int protocol, int sv[2]);
   通过网络socket传递信息  使得两线程通信  sv[0]与sv[1]通信
*/

#include "noncopyable.h"
#include "Timestamp.h"
#include"CurrentThread.h"

#include <functional>
#include <atomic>
#include <memory>
#include <mutex>

class Channel;
class Poller;
/***
 *                   EventLoop
 *       ChannelList            poller 
 *                       epollpoller    (extend pollpoller selectpoller...)          (channelList conut >= channelmap)
 *                     channelMap <fd,channel*>
 *
 */
class EventLoop : noncopyable
{
private:
    /* data */
    using ChannelList = std::vector<Channel *>;
    
    /*EventLoop 是否正在运行*/
    std::atomic_bool looping_;
    // EventLoop 是否退出
    std::atomic_bool quit_;

    // 当前loop 的线程id
    const pid_t threadId_;
    // eventLoop的调度器
    std::unique_ptr<Poller *> poller_;
    // poller所返回的事件集合的时间
    Timestamp pollerRetTime_;

    /**线程通信
     *   当main eventloop 负责监听到一个新的accept时 包装成channel，传递给工作线程。
     *  为什么需要线程通信?
     * 当main loop和work loop 空闲时让它们线程挂起不占用cpu资源,需要时唤醒
     */
    // 轮询选择work loop
    int weakupFd_;
    // 当前的channel
    std::unique_ptr<Channel> weakupChannel_;

    /** 存储所有event loop all channel   channel的集合*/
    ChannelList activeChannels_;
    Channel *currentChannel_;

    /**当前event loop 是否需要执行回调函数*/
    std::atomic_bool callingPendingFunctor_;
    // 存储所有回调函数
    std::vector<Functor> pendingFunctors_;
    //互斥回调函数
    std::mutex mtx_;

    //weakup
    void handleRead();
    //执行回调
    void doPendingFunctor();
public:
    using Functor = std::function<void()>;

    //开启事件循环
    void loop();
    //退出事件循环
    void quit();

    //在当前线程执行cb
    void runInLoop(Functor cb);
    //投递给任务队列 唤醒loop所在线程 暂不执行cb
    void queueInLoop(Functor cb);

    //唤醒loop所在线程
    void weakup();

    //EventLoop -> poller
    void updateChannel(Channel* channel) = 0 ;
    void removeChannel(Channel* channel) = 0;
    bool hasChannel(Channel* target)const;

    //判断当前loop对象是否在自己线程
    bool isInLoopThread(){return threadId_ == CurrentThread::tid();}

    EventLoop(/* args */);
    ~EventLoop();
};



#endif