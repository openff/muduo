#include "Poller.h"
#include"Channel.h"
Poller::Poller(EventLoop *loop) : ownerloop_(loop)
{
}

bool Poller::hasChannel(Channel* target) const
{
    std::unordered_map<int,Channel*>::const_iterator it = Channels_.find(target->fd());
    return it != Channels_.end()  && it->second == target;
}
