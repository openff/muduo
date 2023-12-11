#ifndef TcpConnection_H
#define TcpConnection_H

#include "noncopyable.h"
#include "InetAddr.h"
#include "CallBacks.h"
#include"Timestamp.h"

#include <memory>
#include <cstring>
#include <atomic>

class Channel;
class EventLoop;
class Socket;
class TcpConnection;

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

/***
 * TcpServer  ->  Acceptor   ->有新用户accept拿到clientfd  ->tcpConnect 设置回调->
 * 打包成 Channel -> poller 监管(epoll)-> 事件响应 chnnel回调操作
 *
 *
 */
class TcpConnection : noncopyable, public std::enable_shared_from_this<TcpConnection>
{
public:
    TcpConnection(EventLoop *loop, const std::string &name, int sockefd, const InetAddr &localAddr, const InetAddr &peerAddr);
    ~TcpConnection();

    EventLoop *getLoop() const { return loop_; }
    const std::string name() const { return name_; }
    const InetAddr &localAddr() const { return localAddr; }
    const InetAddr &peerAddr() const { return peerAddr; }

    bool connected() const { return state_ == kConnected; }
    bool disconnected() const { return state_ == kDisconnected; }

    void send(const void *message, int len);
    void send(Buffer *message);

    void shutdown();


    void setConnectionCallback(const ConnectionCallback &cb)
    {
        connectCallback_ = cb;
    }

    void setMessageCallback(const MessageCallback &cb)
    {
        messageCallback_ = cb;
    }

    void setWriteCompleteCallback(const WriteCompleteCallback &cb)
    {
        writeCompleteCallback_ = cb;
    }

    void setHighWaterMarkCallback(const HighWaterMarkCallback &cb, size_t highWaterMark)
    {
        highWriterMarkCallback_ = cb;
        highWaterMark_ = highWaterMark;
    }
    void setCloseCallback(const CloseCallback& cb)
    { closeCallback_ = cb; }
private:
    enum StatuE
    {
        // 已断开连接
        kDisconnected,
        // 正在连接
        kConnecting,
        // 已连接
        kConnected,
        // 正在断开连接
        kDisconnecting
    };
    void handleRead(Timestamp receiveTime);
    void handleWrite();
    void handleClose();
    void hanleError();

    void sendInloop(const void* message,size_t len);
    void shutdownInloop(); 
    /* 此loop不是mainloop 是workloop tcpConnect 在wrokloop工作*/
    EventLoop *loop_;
    const std::string name_;
    bool reading_;
    std::atomic_int state_;

    /**accept -> mainloop  listenfd
     * tcpconnect -> subloop clinetfd
     */
    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;

    const InetAddr localAddr_;
    const InetAddr peerAddr_;

    CloseCallback closeCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    ConnectCallback connectCallback_;
    MessageCallback messageCallback_;
    HighWriterMarkCallback highWriterMarkCallback_

    size_t highWriterMark_; // 双方水位(流量)控制

    Buffer inputBuffer_;
    Buffer outputBuffer_;
};

#endif