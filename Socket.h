#ifndef SOCKET_H
#define SOCKET_H
#include "noncopyable.h"

class InetAddr;

class Socket : noncopyable
{

public:
    explicit Socket(int sockfd) : sockfd_(sockfd)
    {
    }
    ~Socket();

    int fd() const { return sockfd_; }

    void bindAddress(const InetAddr &localaddr);
    void listen();
    int accept(InetAddr *peeraddr);
    void shutdownWrite();

    /**启用/禁用TCP_NODELAY(禁用/启用Nagle算法)。
     * Nagle算法的基本原理是将多个较小的数据包合并成一个更大的数据包进行发送，以减少网络传输的开销。它通过以下两个规则实现：
            1.数据延迟发送（Delayed Acknowledgment）：当应用程序发送一个数据包后，Nagle算法会等待一小段时间，看是否有其他数据要发送。
            如果在这段时间内没有其他数据发送，那么该数据包就会立即发送出去。如果有其他数据要发送，那么Nagle算法会将当前数据包暂存起来，
            并等待其他数据的到达，以便一起发送。
            2.小数据包合并（Packet Coalescing）：当Nagle算法暂存了一个数据包，并且有其他数据到达时，
            它会尝试将这些数据合并成一个更大的数据包进行发送。这样可以减少发送的数据包数量，从而减少网络传输的开销。
    */
    void setTcpNoDelay(bool on);
    /**启用/禁用SO_REUSEADDR  是一个套接字选项，用于在套接字关闭后立即释放绑定的端口，以便其他套接字可以立即重新使用该端口。*/
    void setReuseAddr(bool on);
    /**
     * 启用/禁用SO_REUSEPORT
     * SO_REUSEPORT 是一个套接字选项，用于允许多个套接字绑定到相同的IP地址和端口上。它可以实现负载均衡和并发处理，
     * 允许多个套接字同时监听同一端口，每个套接字都能够独立地接收连接请求。
     */
    void setReusePort(bool on);
    /**
     * 启用/禁用SO_KEEPALIVE
     * SO_KEEPALIVE 是一个套接字选项，用于启用或禁用 TCP keep-alive 功能。TCP keep-alive 是一种机制，
     * 用于检测空闲连接的活动状态，以便在连接空闲超过一定时间后自动关闭它。
     */
    void setKeepAlive(bool on);

private:
    /* data */
    const int sockfd_;
};

#endif