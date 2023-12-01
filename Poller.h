#ifndef Poller_h
#define Poller_h
#include"noncopyable.h"
#include"Timestamp.h"

#include<vector>
#include<unordered_map>

class Channel;
class EventLoop;
class Poller:noncopyable
{
private:
    /* data */
    EventLoop* ownerloop_;
protected:
    using ChannelUnorderMap = std::unordered_map<int,Channel*>;
    ChannelUnorderMap Channels_;
public:
    using ChannelList = std::vector<Channel*>;
    
    /**IO复用提供的统一接口*/
    virtual Timestamp poll(int timeout,ChannelList* activeChannel) = 0 ;
    virtual void updateChannel(Channel* channel) = 0 ;
    virtual void removeChannel(Channel* channel) = 0;

    /**判断当前的poller是否有指定channel*/
    bool hasChannel(Channel* target)const;
    /**根据EventLoop 返回单例poller*/
    static Poller* newDefaultPoller(EventLoop* loop);

    Poller(EventLoop* loop);
    Poller(/* args */) = default;
    virtual ~Poller() = default;
};


#endif