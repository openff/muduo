#include "TcpConnection.h"
#include "Logger.h"
#include "Socket.h"
#include "Channel.h"
#include"Buffer.h"
#include"EventLoop.h"

#include <memory>

static EventLoop *CheckEventLoop(EventLoop *p)
{
    if (p == nullptr)
    {
        LOG_FATAL("TcpServer create error");
        return nullptr;
    }
    return p;
}

TcpConnection::TcpConnection(EventLoop *loop, const std::string &name, int sockefd, const InetAddr &localAddr, const InetAddr &peerAddr)
    : loop_(CheckEventLoop(loop))
    , name_(name)
    , state_(kConnecting)
    , reading_(true)
    , socket_(std::make_unique<Socket>(sockefd))
    , channel_(std::make_unique<Channel>(loop, sockefd))
    , localAddr_(localAddr), peerAddr_(peerAddr), highWaterMark_(64 * 1024 * 1024) // 64M
{
    channel_->setReadEventCallback(
        std::bind(&TcpConnection::handleRead,this,std::placeholders::_1)
    );
    channel_->setwriteEventCallback(
        std::bind(&TcpConnection::handleWrite,this)
    );
    channel_->seterrorEventCallback(
        std::bind(&TcpConnection::hanleError,this)
    );
    channel_->setcloseEventCallback(
        std::bind(&TcpConnection::handleClose,this)
    );

    LOG_INFO("TcpConnection::ctor[%s] at %d fd=%d",name_.data(),this,sockefd);

    socket_->setKeepAlive(true);
}
TcpConnection::~TcpConnection()
{
     LOG_INFO("TcpConnection::ctor[%s] at %d fd=%d",name_.data(),this,sockefd);
}

 void TcpConnection::send(const void *message, int len){

 }
void TcpConnection::send(Buffer *message){

}

void TcpConnection::shutdown(){

}

void TcpConnection::handleRead(Timestamp receiveTime){
    int saveError=0;
    ssize_t res = inputBuffer_.readFd(channel_->fd(),&saveError);
    if (res < 0){
        errno = saveError;
        LOG_ERROR("TcpConnection::handleRead\n");
    }else if (res == 0)
    {
        /* code */
        handleClose();
    }else{
        //已建立连接的用户，有可读事件发生。调用回调操作
        messageCallback_(shared_from_this());
    }
    

}
void TcpConnection::handleWrite(){
    if (channel_->isWriting())
    {
        /* code */
        int saveError=0;
        ssize_t res = outputBuffer_.writeFd(socket_->fd(),&saveError);
        if (n > 0)
        {
            /* code */
            outputBuffer_.retrieve(n);
            if (outputBuffer_.readableBytes() == 0){
                channel_->disableWriting();
                if(writeCompleteCallback_){
                    loop_->queueInLoop(
                        std::bind(writeCompleteCallback_,shared_from_this())
                    );
                }
                if (state_ ==kDisconnecting)
                {
                    /* code */
                    shutdownInloop();
                }
                
            }
        }else{
            LOG_ERROR("TcpConnection::handleWrite\n");
        }
        
    }else{
         LOG_ERROR("TcpConnection::handleWrite      !channel_->isWriting()\n");
    }
    

}
void TcpConnection::handleClose(){

}
void TcpConnection::hanleError(){

}

void TcpConnection::sendInloop(const void* message,size_t len){

}
void TcpConnection::shutdownInloop(){

}