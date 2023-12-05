#ifndef ACCEPTOR_H
#define ACCEPTOR_H
#include"noncopyable.h"
#include"Socket.h"
#include"Channel.h"


class EventLoop;
class InetAddr;


class Acceptor:noncopyable
{
public:
    using NewConnectionCallback = std::function<void(int sockfd,const InetAddr&)>;

    bool listening()const{return listening_;}

    void listen();
    void setNewConnectionCallback(const NewConnectionCallback& cb){
        newConnectionCallback_ = cb;
    }

    Acceptor(EventLoop* loop,const InetAddr& listenAddr,bool reuseport);
    ~Acceptor();
private:
    void handleRead();

    /* data */
    int idleFd_;
    bool listening_;
    Channel acceptChannel_;
    Socket acceptSock_;
    //acceptor使用的是mainloop baseloop_
    EventLoop* loop_;

    //当有新连接时调用的回调函数
    NewConnectionCallback newConnectionCallback_; 
};



#endif