#ifndef TcpServer_H
#define TcpServer_H

#include "Acceptor.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"
#include "noncopyable.h"
#include "TcpConnection.h"
#include "CallBacks.h"

#include <unordered_map>
#include <functional>
#include <string>
#include <atomic>

class TcpServer : noncopyable
{
public:
    using ThreadInitCallback = std::function<void(EventLoop *)>;
    enum option
    {
        kNoReusePort,
        kReusePort,
    };
    void setConnectCallback(const ConnectCallback &cb) { connectCallback_ = cb; }
    void setCloseCallback(const CloseCallback &cb) { closeCallback_ = cb; }
    void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback &cb){ writeCompleteCallback_ = cb; }

    TcpServer(EventLoop *loop,  InetAddr & listenAddr, std::string nameArg,option opt = kNoReusePort);
    ~TcpServer();

    void start();
    //设置线程数量
    void setThreadNum(int count);
private:
    /* 回调 */
    void newConnection(int sockfd,const InetAddr& peerAddr);
    void removeConnection(const TcpConnectionPtr& conn);
    void removeConnectionInLoop(const TcpConnectionPtr& conn);

    using ConnectionMap = std::unordered_map<std::string, TcpConnection>;
    ConnectCallback connectCallback_;
    CloseCallback closeCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;

    // loop线程初始化的回调函数
    ThreadInitCallback threadInitCallback_;

    std::atomic_int32_t started_;
    int nextConnId_;
    // 保存的所有连接
    ConnectionMap conntions_;

    // baseloop  mainloop用户定义的
    EventLoop *loop_;
    const std::string ipPort_;
    const std::string name_;
    // 运行在mainloop 任务就是监听新连接任务事件
    std::unique_ptr<Acceptor> acceptor_;
    // one loop pee thread
    std::shared_ptr<EventLoopThreadPool> threadPool_;
};



#endif