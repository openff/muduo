#ifndef Channel_H
#define Channel_H

#include "noncopyable.h"
#include "Timestamp.h"
#include"EpollPoller.h"

#include <functional>
#include <memory>
#include <sys/epoll.h>
class EventLoop;

/***
 * Channel 理解为通道，封装了sockfd和其感兴趣的event，如EPOLLIN,EPOLLOUT事还绑定了poller返回的具体事件
 */
class Channel : noncopyable
{

public:
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(Timestamp)>;
    // 设置epoll的事件回调
    void setReadEventCallback(ReadEventCallback cb);
    void setwriteEventCallback(EventCallback cb);
    void setcloseEventCallback(EventCallback cb);
    void seterrorEventCallback(EventCallback cb);

    int fd() const { return fd_; }
    int events() const { return events_; }
    void set_retevent(int ret) { ret = retevent_; }
    bool isNoneEvent() const { return (events_ == kNoneEvent); }
    // 设置响应的事件状态
    void enableReading()
    {
        events_ |= kReadEvent;
        update();
    }
    void disableReading()
    {
        events_ &= ~kReadEvent;
        update();
    }
    void enableWriting()
    {
        events_ |= kWriteEvent;
        update();
    }
    void disableWriting()
    {
        events_ &= ~kWriteEvent;
        update();
    }
    void disableAll()
    {
        events_ = kNoneEvent;
        update();
    }

    // 返回状态
    bool isWriting() const { return events_ & kWriteEvent; }
    bool isReading() const { return events_ & kReadEvent; }
    
    int index() const { return index_; }
    void set_index(int idx) { index_ = idx; }

    EventLoop *ownerLoop() { return loop_; }

    /**在EventLoop中调用 删除当前channel */
    void remove();
    /**处理poller回应的事件*/
    void handleEvent(Timestamp receiveTime);
    /** 防止在执行回调的时候销毁对象 */
    void tie(const std::shared_ptr<void> &);

    Channel(EventLoop *loop, int fd);
    ~Channel();

private:
    
    // 表示当前channel fd的状态
    static const int kNoneEvent{0};
    static const int kReadEvent{EPOLLIN | EPOLLPRI};
    static const int kWriteEvent{EPOLLOUT};

    EventLoop *loop_;
    const int fd_; // poller管理的sockfd
    int events_;   // 注册处理的事件
    int retevent_; // 触发感兴趣的事件
    int index_;

    // 跨线程监视 防止remove
    std::weak_ptr<void> tie_;
    bool tied_;

    // callback
    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;

    /* data */
    /**通过EventLoop在poller调用 更新epoll状态 (epoll_ctl_(add,mod,del))*/
    void update();

    void handleEventWithGuard(Timestamp receiveTime);
};

#endif