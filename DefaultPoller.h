#include"Poller.h"
#include"EpollPoller.h"

/**
 *  父类 根据环境配置返回对应实例对象
*/
 std::unique_ptr<Poller>  Poller::newDefaultPoller(EventLoop* loop){
    if (getenv("MUDUO_USE_POLL"))
    {
        /* 返回poll实例 */
        return nullptr;
    }else{
        /**返回epoll实例*/
        std::unique_ptr<Poller> p = std::make_unique<EpollPoller>(loop);
        return p;
    }
    
 }
