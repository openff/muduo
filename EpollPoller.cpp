#include "EpollPoller.h"
#include "Logger.h"
#include "Channel.h"

#include <error.h>
#include <unistd.h>
#include <assert.h>
#include <cstring>

// channel 未从list添加到channelmap
const int kNew = -1;
// channel 已从list添加到map
const int kAdded = 1;
// channel从map删除了
const int kDelete = 2;
/***
 *                  EventLoop
 *       ChannelList            poller 
 *                         epollpoller   poll poller...          (channelList conut >= channelmap)
 *                     channelMap <fd,channel*>
 *
 */
EpollPoller::EpollPoller(EventLoop *loop) : Poller(loop), events_(kInitEventListSize), epollfd_(epoll_create(1))
{
    if (epollfd_ < 0)
    {
        /* code */
        LOG_FATAL("epoll_create error num:%d \n", errno);
    }
}
EpollPoller::~EpollPoller()
{
    close(epollfd_);
}
void EpollPoller::fillActiveChannels(int numEvents, ChannelList *activeChannels) const
{
    for (size_t i = 0; i < numEvents; i++)
    {
        /** 
         * 处理响应集合事件
         */
        Channel *channel = static_cast<Channel*>(events_[i].data.ptr);
        //设置发生事件 并放入到需事件集合
        channel->set_retevent(events_[i].events); 
        activeChannels->push_back(channel);
    }
}
/**epoll_wait */
Timestamp EpollPoller::poll(int timeout, ChannelList *activeChannel)
{
    LOG_INFO("fd total count:%ld", Channels_.size());
    /**监听一组事件集合*/
    int lenght = events_.size();
    /**返回事件响应的个数*/
    int numEvents = epoll_wait(epollfd_, &*events_.begin(), lenght, timeout);
    
    int savedError = errno;
    Timestamp now(Timestamp::now());
    if (numEvents > 0)
    {
         LOG_INFO("poll count [numEvents]:%d\n", numEvents);
  
        fillActiveChannels(numEvents, activeChannel);
        //扩容操作
        if (numEvents == lenght){
            events_.resize(2 * lenght);
        }else if (numEvents == 0)
        {
           //no eveting
           LOG_INFO("no eveting\n");
        }
        else{
                if(savedError != EINTR){
                    /**外部中断*/
                    LOG_ERROR("EINTR : %d\n",savedError);
                }
        }
        
    }
    return now;
}
/**
 * epoll_ctl前置设参  根据index 更新对应状态
 */
void EpollPoller::updateChannel(Channel *channel)
{
    const int index = channel->index();
    LOG_INFO("update channel  : \t[fd]:  %d\t[event]:%d  [index]:%d\n",channel->fd(), channel->events(), channel->index());
    if (index == kNew || index == kDelete)
    {
        // 连接 or 离线
        int fd = channel->fd();
        if (index == kNew)
        {
            /* 连接 ADD 如果该容器没有channel则放入*/
            assert(!Poller::hasChannel(channel));
            Channels_[fd] = channel;
        }
        else
        {
            /**离线*/
            // assert(Poller::hasChannel(channel));
        }
        /* Poller中无channel 情况 加入 */
        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }
    else /**已经注册了 chanelmap 有该 channel*/
    {
        // epoll mod & del
        int fd = channel->fd();
        assert(Poller::hasChannel(channel) && index == kAdded);
        if (channel->isNoneEvent())
        {
            /* 当前channel没有关注任何Event 则del*/
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDelete);
        }
        else
        {
            /*否则进行mod*/
            update(EPOLL_CTL_MOD, channel);
        }
    }
}
/**
 * epoll_ctl投递事件
 */
void EpollPoller::update(int operation, Channel *channel)
{
    int fd = channel->fd();
    /**创建一个事件*/
    epoll_event event;
    std::memset(&event, 0, sizeof(epoll_event));
    event.events = channel->events();
    event.data.ptr = channel;
    event.data.fd = fd;

    /**开始投递事件给epoll*/
    if (epoll_ctl(epollfd_, operation, fd, &event) < 0)
    {
        /**投递时出错处理*/
        if (operation == EPOLL_CTL_DEL)
        {
            LOG_ERROR("epoll_ctling error op = fd :%d", fd);
        }
        else
        {
            LOG_FATAL("epoll_ctling error op (add/mod) fd :%d", fd);
        }
    }
}
/**在channelmap 删除指定channel并ctl_del */
void EpollPoller::removeChannel(Channel *channel)
{
   
    int fd = channel->fd();
    int index = channel->index();
    
    size_t n = Channels_.erase(fd);
    if (index == kAdded) 
    {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->set_index(kNew);
}
const char *EpollPoller::operatorToString(int op)
{
    switch (op)
    {
    case EPOLL_CTL_ADD:
        return "ADD";
    case EPOLL_CTL_DEL:
        return "DEL";
    case EPOLL_CTL_MOD:
        return "MOD";
    default:
        LOG_FATAL("Unknown Operation\n");
        return "Unknown Operation";
    }
}
