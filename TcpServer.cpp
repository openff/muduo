#include "TcpServer.h"
#include"Logger.h"
#include"InetAddr.h"

#include<memory>

static EventLoop* CheckEventLoop(EventLoop* p ){
    if (p == nullptr){
        LOG_FATAL("TcpServer create error");
        return nullptr;
    }
    return p;
}

TcpServer::TcpServer(EventLoop *loop,  InetAddr & listenAddr, std::string nameArg,option opt)
    :loop_(CheckEventLoop(loop))
    ,ipPort_(listenAddr.toIpPort())
    ,name_(nameArg)
    ,acceptor_(std::make_unique<Acceptor>(loop,listenAddr,opt))
    ,threadPool_(std::make_shared<EventLoopThreadPool>(loop,name_))
    ,nextConnId_(1)
{
    //当有用户连接 启用该回调  acceptor类中handleRead调用
    acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection,this,std::placeholders::_1,std::placeholders::_2));

}
TcpServer::~TcpServer()
{
}

void TcpServer::start()
{
    if (started_++ == 0)
    {
        /* 启动线程 和时间循环 */
        threadPool_->start(threadInitCallback_);
        loop_->runInLoop(std::bind(&Acceptor::listen,acceptor_.get()));
    }
    
}
// 设置线程数量
void TcpServer::setThreadNum(int count)
{
    threadPool_->setThreadNum(count);
}

/* 回调 */
void TcpServer::newConnection(int sockfd, const InetAddr &peerAddr)
{
    /**
     * 根据轮询算法选择一个workloop
     * 唤醒workloop
     * 把connfd封装成channel 投递给workloop
     * 
    */
}
void TcpServer::removeConnection(const TcpConnectionPtr &conn)
{
}
void TcpServer::removeConnectionInLoop(const TcpConnectionPtr &conn)
{
}
