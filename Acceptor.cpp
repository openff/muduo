#include "Acceptor.h"
#include "Socket.h"
#include "Logger.h"
#include"InetAddr.h"

#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include<unistd.h>
// 创建一个异步socket
static int createNonBlocking()
{

    int sock = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if (sock < 0)
    {
        LOG_FATAL("acceptor createNonblock error");
    }
    return sock;
}

Acceptor::Acceptor(EventLoop *loop, const InetAddr &listenAddr, bool reuseport)
    : loop_(loop), acceptSock_(createNonBlocking()), acceptChannel_(loop, acceptSock_.fd()), listening_(false)
{   
    acceptSock_.setReuseAddr(true);
    acceptSock_.setReusePort(true);
    acceptSock_.bindAddress(listenAddr);
    //有新链接 ->执行回调 ->connfd-》channel-》subloop管理
    acceptChannel_.setReadEventCallback(std::bind(&Acceptor::handleRead,this));

}
Acceptor::~Acceptor() {
    acceptChannel_.disableAll();
    acceptChannel_.remove();

}

void Acceptor::listen()
{
    listening_=true;
    acceptSock_.listen();
    acceptChannel_.enableReading();
}

void Acceptor::handleRead() {
    InetAddr peerAddr;
    int connfd = acceptSock_.accept(&peerAddr);
    if (connfd >= 0)
    {
        /* 调用用户设置的回调 无则离线*/
        if (newConnectionCallback_)
        {
            newConnectionCallback_(connfd,peerAddr);
        }else   {
            ::close(connfd);
        }
        
    }else{
        /**accept出错*/
        LOG_ERROR("handleRead accept error");
        if (errno == EMFILE){
            /**当前内存已满 */
        }
    }
}