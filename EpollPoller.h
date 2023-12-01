#ifndef EpollPoller_H
#define EpollPoller_H

#include "Poller.h"

#include<sys/epoll.h>
#include <vector>

class Channel;
/**
 * epoll动作
 * epoll_create
 * epoll_ctl
 * epoll_wait
 */
class EpollPoller : public Poller
{
private:
    using EventList = std::vector<epoll_event>;
    /* data */
    int epollfd_{0};
     //处理的事件集合
    EventList events_;
    static const int kInitEventListSize{16};
    // 填写活跃连接？
    void fillActiveChannels(int numEvents, ChannelList *activeChannels) const;
    void update(int operation, Channel *channel);
    const char* operatorToString(int op);
public:
    /**IO复用提供的统一接口*/
    /**epoll_wait*/
    Timestamp poll(int timeout, ChannelList *activeChannel) override;
    void updateChannel(Channel *channel) override;
    void removeChannel(Channel *channel) override;

    EpollPoller(EventLoop *loop);
    EpollPoller(/* args */) = default;
    ~EpollPoller() override;
};

#endif