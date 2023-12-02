#include "Channel.h"
#include "Logger.h"
#include"EventLoop.h"

#include <iostream>
#include <memory>
#include <sys/epoll.h>
/**
 * 状态图：
 * EventLoop 子-> 管理多个channel  和poller调度器
 *
 */
Channel::Channel(EventLoop *loop, int fd) : loop_(loop), fd_(fd), events_(0), retevent_(0), tied_(false),index_(-1)
{
}
Channel::~Channel()
{
}
void Channel::setReadEventCallback(ReadEventCallback cb)
{
    readCallback_ = std::move(cb);
}
void Channel::setwriteEventCallback(EventCallback cb)
{
    writeCallback_ = std::move(cb);
}
void Channel::setcloseEventCallback(EventCallback cb)
{
    closeCallback_ = std::move(cb);
}
void Channel::seterrorEventCallback(EventCallback cb)
{
    errorCallback_ = std::move(cb);
}

/** 防止在执行回调的时候销毁对象 */
void Channel::tie(const std::shared_ptr<void> &obj)
{
    tie_ = obj;
    tied_ = true;
}
/**在EventLoop中调用 删除当前channel */
void Channel::remove()
{
    loop_->removeChannel(this);
}
/**通过EventLoop在poller调用 更新epoll状态 (epoll_ctl_(add,mod,del))*/
void Channel::update()
{
   loop_->updateChannel(this);
}

/**处理poller回应的事件*/
void Channel::handleEvent(Timestamp receiveTime)
{
    if (tied_)
    {
        std::shared_ptr<void> guard = tie_.lock();
        if (guard)
        {
            handleEventWithGuard(receiveTime);
        }
    }
    else
    {
        handleEventWithGuard(receiveTime);
    }
}

void Channel::handleEventWithGuard(Timestamp receiveTime)
{
    /**
     * poll状态描述符
        POLLIN：文件描述符可读。
        POLLOUT：文件描述符可写。
        POLLPRI：有紧急数据可读。
        POLLERR：文件描述符发生错误。
        POLLHUP：文件描述符挂起或关闭。
        POLLNVAL：无效的文件描述符
     * epoll 状态描述符
        EPOLLIN：文件描述符可读。
        EPOLLOUT：文件描述符可写。
        EPOLLPRI：有紧急数据可读。
        EPOLLERR：文件描述符发生错误。
        EPOLLHUP：文件描述符挂起或关闭。
        EPOLLRDHUP：文件描述符的对端关闭连接或关闭写入端。
        EPOLLONESHOT：设置文件描述符为一次性触发模式。
        EPOLLET：使用边缘触发模式。
    */

    LOG_INFO("handle event retevent status:%d\n", retevent_);
    if ((retevent_ & EPOLLHUP) && !(retevent_ & EPOLLIN))
    {
        /*文件描述符挂起或关闭。*/
        if (closeCallback_)
            closeCallback_();
    }

    if (retevent_ & EPOLLERR)
    {
        /* ERROR */
        if (errorCallback_)
        {
            errorCallback_();
        }
    }

    if (retevent_ & EPOLLOUT)
    {
        /* read */
        if (writeCallback_)
        {
            writeCallback_();
        }
    }

    if (retevent_ & (EPOLLIN | EPOLLPRI))
    {
        /* READ */
        if (readCallback_)
        {
            readCallback_(receiveTime);
        }
    }
}